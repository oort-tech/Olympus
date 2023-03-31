#include "type.hpp"

namespace dev
{
	/// typeRegex parses the abi sub types
	boost::regex typeRegex("([a-zA-Z]+)(([0-9]+)(x([0-9]+))?)?");

	void from_json(const nlohmann::json& j, ArgumentMarshaling& p)
	{
		if (j.count("name"))
			j.at("name").get_to(p.Name);
		if (j.count("type"))
			j.at("type").get_to(p.Type);
		if (j.count("internalType"))
			j.at("internalType").get_to(p.InternalType);
		if (j.count("components"))
		{
			for (auto &component : j.at("components"))
			{
				ArgumentMarshaling in;
				from_json(component, in);
				p.Components.push_back(in);
			}
		}
		if (j.count("indexed"))
			j.at("indexed").get_to(p.Indexed);
	}

	/// isDynamicType returns true if the type is dynamic.
	/// The following types are called ¡°dynamic¡±:
	/// * bytes
	/// * string
	/// * T[] for any T
	/// * T[k] for any dynamic T and any k >= 0
	/// * (T1,...,Tk) if Ti is dynamic for some 1 <= i <= k
	bool isDynamicType(Type const & t)
	{
		if (t.T == ValueType::TupleTy)
		{
			for (auto elem : t.TupleElems)
			{
				if (isDynamicType(elem))
					return true;
			}
			return false;
		}
		return t.T == ValueType::StringTy || t.T == ValueType::BytesTy || t.T == ValueType::SliceTy || (t.T == ValueType::ArrayTy && isDynamicType(*t.Elem));
	}

	/// getTypeSize returns the size that this type needs to occupy.
	/// We distinguish static and dynamic types. Static types are encoded in-place
	/// and dynamic types are encoded at a separately allocated location after the
	/// current block.
	/// So for a static variable, the size returned represents the size that the
	/// variable actually occupies.
	/// For a dynamic variable, the returned size is fixed 32 bytes, which is used
	/// to store the location reference for actual value storage.
	int getTypeSize(Type const& t)
	{
		if (t.T == ValueType::ArrayTy && !isDynamicType(*t.Elem))
		{
			/// Recursively calculate type size if it is a nested array
			if ((*(t.Elem)).T == ValueType::ArrayTy || (*(t.Elem)).T == ValueType::TupleTy)
			{
				return t.Size * getTypeSize(*t.Elem);
			}
			return t.Size * 32;
		}
		else if (t.T == TupleTy && !isDynamicType(t))
		{
			int total = 0;
			for (auto elem : t.TupleElems) {
				total += getTypeSize(elem);
			}
			return total;
		}
		return 32;
	}

	int lengthPrefixPointsTo(int index, dev::bytes const & output)
	{
		int outputLength = output.size();
		///data offset
		///first 32 bytes record dynamic data length.
		int offset = fromBigEndian<int>(dev::bytes(output.data() + index, output.data() + index + 32));
		if (offset + 32 > outputLength)
			BOOST_THROW_EXCEPTION(dev::FailedABI());
		///dynamic data length.
		int length = fromBigEndian<int>(dev::bytes(output.data() + offset, output.data() + offset + 32));
		if (offset + length > outputLength)
			BOOST_THROW_EXCEPTION(dev::FailedABI());
		return offset;
	}

	///NewType creates a new reflection type of abi type given in t.
	std::shared_ptr<Type> NewType(std::string const& _t, std::string const& _internalType, ArgumentMarshalings components)
	{
		/// check that array brackets are equal if they exist
		if (boost::contains(_t, "[") != boost::contains(_t, "]"))
			BOOST_THROW_EXCEPTION(dev::FailedABI());

		Type typ;
		typ.stringKind = _t;
		/// if there are brackets, get ready to go into slice/array mode and
		/// recursively create the type
		if (boost::contains(_t, "["))
		{
			/// Note internalType can be empty here.
			std::string subInternal = _internalType;
			auto p1 = _internalType.find_last_of("[");
			if (p1 != std::string::npos)
			{
				subInternal = _internalType.substr(0, p1);
			}
			/// recursively embed the type
			auto p2 = _t.find_last_of("[");
			auto embeddedType = NewType(_t.substr(0, p2), subInternal, components);
			/// grab the last cell and create a type from there
			auto sliced = _t.substr(p2);
			/// grab the slice size with regexp
			boost::smatch parsed;
			if (!boost::regex_search(sliced, parsed, boost::regex("[0-9]+")))
				parsed = boost::smatch();
			if (parsed.size() == 0)
			{
				/// is a slice
				typ.T = ValueType::SliceTy;
				typ.Elem = embeddedType;
				typ.stringKind = embeddedType->stringKind + sliced;
			}
			else if (parsed.size() == 1)
			{
				/// is an array
				typ.T = ArrayTy;
				typ.Elem = embeddedType;
				typ.Size = std::stoi(parsed[0].str());
				typ.stringKind = embeddedType->stringKind + sliced;
			}
			else
				BOOST_THROW_EXCEPTION(dev::FailedABI());
			return std::make_shared<Type>(typ);
		}

		/// parse the type and size of the abi-type. such as bytes10 parse to ["bytes10", "bytes", "10", "10" ,"", ""]
		boost::smatch parsed;
		if (!boost::regex_match(_t, parsed, typeRegex) || !parsed.size())
			BOOST_THROW_EXCEPTION(dev::FailedABI());
		int varSize = 0;
		if (parsed[3].str().length()) ///like bytes10
			varSize = boost::lexical_cast<int>(parsed[2].str());
		else
		{
			if (parsed[0].str() == "uint" || parsed[0].str() == "int")
				BOOST_THROW_EXCEPTION(dev::FailedABI());
		}

		/// varType is the parsed abi type
		auto varType = parsed[1].str();
		if ("int" == varType)
		{
			typ.Size = varSize;
			typ.T = ValueType::IntTy;
		}
		else if ("uint" == varType)
		{
			typ.Size = varSize;
			typ.T = ValueType::UintTy;
		}
		else if ("bool" == varType)
		{
			typ.T = ValueType::BoolTy;
		}
		else if ("address" == varType)
		{
			typ.Size = 20;
			typ.T = ValueType::AddressTy;
		}
		else if ("string" == varType)
		{
			typ.T = ValueType::StringTy;
		}
		else if ("bytes" == varType)
		{
			if (varSize == 0)
			{
				typ.T = ValueType::BytesTy;
			}
			else
			{
				typ.T = ValueType::FixedBytesTy;
				typ.Size = varSize;
			}
		}
		else if ("tuple" == varType)///Parameter contains custom structures.
		{
			std::string expression = "(";
			std::unordered_set<std::string> used;
			int idx = 0;
			size_t compontSize = components.size();
			for (size_t idx = 0; idx < compontSize; idx++)
			{
				auto c = components[idx];
				auto cType = NewType(c.Type, c.InternalType, c.Components);
				auto name = ToCamelCase(c.Name);
				if (name == "")
					BOOST_THROW_EXCEPTION(dev::FailedABI());

				auto fieldName = ResolveNameConflict(name, [used](std::string const& _name) { return used.count(_name); });
				used.emplace(fieldName);
				if (!isValidFieldName(fieldName))
					BOOST_THROW_EXCEPTION(dev::FailedABI());

				typ.TupleElems.push_back(*cType);
				typ.TupleRawNames.push_back(c.Name);

				expression += cType->stringKind;
				if (idx != compontSize-1)
					expression += ",";
			}
			expression += ")";
			typ.T = ValueType::TupleTy;
			typ.stringKind = expression;

			const std::string structPrefix = "struct ";
			/// After solidity 0.5.10, a new field of abi "internalType"
			/// is introduced. From that we can obtain the struct name
			/// user defined in the source code.
			if (_internalType != "" && boost::starts_with(_internalType, structPrefix))
			{
				/// Foo.Bar type definition is not allowed ,
				/// convert the format to FooBar
				typ.TupleRawName = _internalType;
				typ.TupleRawName.erase(0, structPrefix.length());
				boost::algorithm::replace_all(typ.TupleRawName, ".", "");
			}
		}
		else if ("function" == varType)
		{
			typ.T = ValueType::FunctionTy;
			typ.Size = 24;
		}
		else
			BOOST_THROW_EXCEPTION(dev::FailedABI());

		return std::make_shared<Type>(typ);
	}
}