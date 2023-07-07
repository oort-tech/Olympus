// Olympus: mcp C++ client, tools and libraries.
// Licensed under the GNU General Public License, Version 3.
/// @file
/// This file defines contract abi interface type.

#pragma once

#include "utils.hpp"
#include <libdevcore/Common.h>
#include <mcp/common/json.hpp>
#include <boost/algorithm/string.hpp>
#include <libdevcore/Exceptions.h>

namespace dev
{
	/// Type enumerator
	enum ValueType : byte
	{
		IntTy = 0,
		UintTy,
		BoolTy,
		StringTy,
		SliceTy,
		ArrayTy,
		TupleTy,
		AddressTy,
		FixedBytesTy,
		BytesTy,
		HashTy,
		FixedPointTy,
		FunctionTy
	};

	class ArgumentMarshaling
	{
	public:
		std::string Name;
		std::string Type;
		std::string InternalType;
		std::vector<ArgumentMarshaling> Components;
		bool Indexed = false;
	};
	using ArgumentMarshalings = std::vector<ArgumentMarshaling>;
	void from_json(const nlohmann::json& j, ArgumentMarshaling& p);
	

	/// Type is the reflection of the supported argument type.
	class Type
	{
	public:
		Type() {};
		/// requireLengthPrefix returns whether the type requires any sort of length
		/// prefixing.
		bool requiresLengthPrefix() {
			return T == ValueType::StringTy || T == ValueType::BytesTy || T == ValueType::SliceTy;
		}

		static dev::bytes encode(uint8_t _value) { return dev::bytes(31, 0) + dev::bytes{ _value }; }
		static dev::bytes encode(bool _value) { return encode(uint8_t(_value)); }
		static dev::bytes encode(dev::u256 const& _value) { return toBigEndian(_value); }
		static dev::bytes encode(int _value) { return encode(dev::u256(_value)); }
		static dev::bytes encode(size_t _value) { return encode(dev::u256(_value)); }
		static dev::bytes encode(dev::h256 const& _value) { return _value.asBytes(); }
		static dev::bytes encode(dev::h160 const& _value) { return encode(dev::h256(_value, dev::h256::AlignRight)); }///RightPad
		dev::bytes encode(dev::bytes const& _value)
		{
			dev::bytes padding = dev::bytes((32 - _value.size() % 32) % 32, 0);
			if (requiresLengthPrefix())
				return encode(_value.size()) + _value + padding;///encode with length;
			else
				return _value + padding;
		}
		dev::bytes encode(std::string const& _value) { return encode(dev::asBytes(_value)); }
		dev::bytes encode(char const* _value) { return encode(std::string(_value)); }

		///for boost::tuple
		void encode(const boost::tuples::null_type&, int& offset, dev::bytes& ret, dev::bytes& tail, int index);///tuple stopped function
		template <typename Tuple>
		void encode(const Tuple& _value, int& offset, dev::bytes& ret, dev::bytes& tail, int index = 0);
		template <typename Tuple>
		dev::bytes encode(Tuple const& _value);
		///for vector
		template <typename TY>
		dev::bytes encode(std::vector<TY> const& _value);

		///decoder
		static void decode(dev::bytes const& _in, uint8_t& _value) { _value = _in.at(31); }
		static void decode(dev::bytes const& _in, bool& _value) { uint8_t a; decode(_in, a); if (a == 0) _value = false; else _value = true; }
		static void decode(dev::bytes const& _in, dev::u256& _value) { _value = fromBigEndian<dev::u256>(_in); }
		static void decode(dev::bytes const& _in, int& _value) { _value = fromBigEndian<int>(_in); }
		static void decode(dev::bytes const& _in, size_t& _value) { _value = fromBigEndian<size_t>(_in); }
		static void decode(dev::bytes const& _in, dev::h256& _value) { _value = dev::h256(_in); }
		static void decode(dev::bytes const& _in, dev::h160& _value) { _value = dev::h160(_in, dev::h160::AlignRight); }///RightPad
		void decode(dev::bytes const& _in, dev::bytes& _value)
		{
			int length = Size;
			if (requiresLengthPrefix())
			{
				length = fromBigEndian<int>(dev::bytes(_in.data(), _in.data() + 32));
				if (length + 32 > _in.size())
					BOOST_THROW_EXCEPTION(dev::FailedABI());
				_value = dev::bytes(_in.data() + 32, _in.data() + 32 + length);
			}
			else
				_value = dev::bytes(_in.data(), _in.data() + length);
		}
		void decode(dev::bytes const& _in, std::string& _value) { dev::bytes r; decode(_in, r); _value = dev::asString(r); }

		///for boost::tuple
		void decode(dev::bytes const& in, const boost::tuples::null_type&, int virtualArgs, int index);///tuple stopped function
		template <typename Tuple>
		void decode(dev::bytes const& in, Tuple& _value, int virtualArgs = 0, int index = 0);
		///for vector
		template <class TY>
		void decode(dev::bytes const& _in, std::vector<TY>& _value);

		///isDynamicType returns true if the type is dynamic.
		///The following types are called ¡°dynamic¡±:
		///* bytes
		///* string
		///* T[] for any T
		///* T[k] for any dynamic T and any k >= 0
		///* (T1,...,Tk) if Ti is dynamic for some 1 <= i <= k
		bool isDynamicType();
		/// getTypeSize returns the size that this type needs to occupy.
		/// We distinguish static and dynamic types. Static types are encoded in-place
		/// and dynamic types are encoded at a separately allocated location after the
		/// current block.
		/// So for a static variable, the size returned represents the size that the
		/// variable actually occupies.
		/// For a dynamic variable, the returned size is fixed 32 bytes, which is used
		/// to store the location reference for actual value storage.
		int getTypeSize();
		/// lengthPrefixPointsTo interprets a 32 byte slice as an offset and then determines which indices to look to decode the type.
		int lengthPrefixPointsTo(int index, dev::bytes const& output);

		template<typename Arg>
		dev::bytes Pack(Arg const& arg);

		template<typename Arg>
		void Unpack(int const& index, dev::bytes const& output, Arg& arg);

		std::shared_ptr<Type>		Elem = nullptr;
		int			Size = 0;
		ValueType	T; /// Our own type checking

		std::string	stringKind; /// holds the unparsed string for deriving signatures

		/// Tuple relative fields
		std::string	TupleRawName;       /// Raw struct name defined in source code, may be empty.
		std::vector<Type> TupleElems;      /// Type information of all tuple fields
		std::vector<std::string>	TupleRawNames;     /// Raw field name of all tuple fields
	};

	///tuple stopped function
	inline void Type::encode(const boost::tuples::null_type&, int& offset, dev::bytes& ret, dev::bytes& tail, int index)
	{
		if (TupleElems.size() > index)///missing parameter
			BOOST_THROW_EXCEPTION(dev::FailedABI());
	}
	template<typename Tuple>
	inline void Type::encode(const Tuple & _value, int & offset, dev::bytes & ret, dev::bytes & tail, int index)
	{
		if (TupleElems.size() == index)///missing parameter
			BOOST_THROW_EXCEPTION(dev::FailedABI());
		Type elem = TupleElems[index];
		auto field = _value.get_head();
		dev::bytes val = elem.Pack(field);
		if (elem.isDynamicType())
		{
			ret += encode(offset);
			tail += val;
			offset += val.size();
		}
		else
			ret += val;

		index++;///next value
		encode(_value.get_tail(), offset, ret, tail, index);
	}

	template<typename Tuple>
	inline dev::bytes Type::encode(Tuple const & _value)
	{
		if (T != ValueType::TupleTy)///tuple
			BOOST_THROW_EXCEPTION(dev::FailedABI());
		int offset = 0;
		dev::bytes ret;
		dev::bytes tail;
		for (auto elem : TupleElems)
			offset += elem.getTypeSize();
		encode(_value, offset, ret, tail);
		return ret + tail;
	}

	template<typename TY>
	inline dev::bytes Type::encode(std::vector<TY> const & _value)
	{
		dev::bytes ret;
		if (requiresLengthPrefix())
			ret = encode(_value.size());///encode with length;

										/// calculate offset if any
		int offset = 0;
		bool offsetReq = Elem->isDynamicType();
		if (offsetReq)
			offset = Elem->getTypeSize() * _value.size();

		dev::bytes tail;
		for (auto const& v : _value)
		{
			dev::bytes val = Elem->Pack(v);
			if (!offsetReq)
			{
				ret += val;
				continue;
			}
			ret += encode(offset);
			offset += val.size();
			tail += val;
		}
		return ret + tail;
	}

	///tuple stopped function
	inline void Type::decode(dev::bytes const& in, const boost::tuples::null_type&, int virtualArgs, int index) {}

	template<typename Tuple>
	inline void Type::decode(dev::bytes const & in, Tuple & _value, int virtualArgs, int index)
	{
		if (TupleElems.size() == index)///missing parameter
			BOOST_THROW_EXCEPTION(dev::FailedABI());
		Type elem = TupleElems[index];

		elem.Unpack((index + virtualArgs) * 32, in, _value.get_head());

		index++;
		if (elem.T == ValueType::ArrayTy && !elem.isDynamicType())///array but static data, put data in the place 
			virtualArgs = virtualArgs + elem.Size - 1;
		decode(in, _value.get_tail(), virtualArgs, index);
	}

	template<class TY>
	inline void Type::decode(dev::bytes const & _in, std::vector<TY>& _value)
	{
		dev::bytes in(_in);
		int size = Size;
		if (requiresLengthPrefix())
		{
			size = fromBigEndian<int>(dev::bytes(in.data(), in.data() + 32));
			in = dev::bytes(in.data() + 32, in.data() + in.size());
		}

		/// calculate offset if any
		for (size_t i = 0; i < size; i++)
		{
			int index = i * 32;
			TY v;
			Elem->Unpack(index, in, v);
			_value.push_back(v);
		}
	}

	template<typename Arg>
	inline dev::bytes Type::Pack(Arg const& arg)
	{
		// typeCheck checks that the given reflection value can be assigned to the reflection
		// type in t.
		//if (typeCheck(T, arg))
		//	BOOST_THROW_EXCEPTION(dev::FailedABI());
		return encode(arg);
	}

	/// Unpack parses the output bytes and recursively assigns the value of these bytes
	/// into a go type with accordance with the ABI spec.
	template<typename Arg>
	inline void Type::Unpack(int const& index, dev::bytes const& output, Arg & arg)
	{
		if (index+32 > output.size())
			BOOST_THROW_EXCEPTION(dev::FailedABI());
		dev::bytes returnOutput;
		int begin;
		/// if we require a length prefix, find the beginning word and size returned.
		if (requiresLengthPrefix())
		{
			begin = lengthPrefixPointsTo(index, output);
		}
		else
			returnOutput = dev::bytes(output.data() + index, output.data() + index + 32);

		switch (T)
		{
		///requiresLengthPrefix, if we require a length prefix, find the beginning word and size returned.
		case dev::StringTy:
		case dev::BytesTy:
		{
			///first is beginning word,second is size.
			returnOutput = dev::bytes(output.data() + begin, output.data() + output.size());
			decode(returnOutput, arg);
			break;
		}
		case dev::SliceTy:
		{
			returnOutput = dev::bytes(output.data() + begin, output.data() + output.size());
			decode(returnOutput, arg);
			break;
		}
		case dev::ArrayTy:
		{
			if (Elem->isDynamicType())
			{
				int offset = fromBigEndian<int>(returnOutput);
				if (offset > output.size())
					BOOST_THROW_EXCEPTION(dev::FailedABI());
				returnOutput = dev::bytes(output.data() + offset, output.data() + output.size());
			}
			else
				returnOutput = dev::bytes(output.data() + index, output.data() + output.size());
			decode(returnOutput, arg);
			break;
		}
		case dev::IntTy:
		case dev::UintTy:
		case dev::BoolTy:
		case dev::AddressTy:
		case dev::HashTy:
		case dev::FixedBytesTy:
		case dev::FunctionTy:
		{
			dev::bytes(output.data() + index, output.data() + index + 32);
			decode(returnOutput, arg);
			break;
		}
		case dev::TupleTy:
		{
			if (isDynamicType())
			{
				int offset = fromBigEndian<int>(returnOutput);
				if (offset > output.size())
					BOOST_THROW_EXCEPTION(dev::FailedABI());
				returnOutput = dev::bytes(output.data() + offset, output.data() + output.size());
			}
			else
				returnOutput = dev::bytes(output.data() + index, output.data() + output.size());
			decode(returnOutput, arg);
			break;
		}
		case dev::FixedPointTy:///not support
			break;
		default:
			break;
		}
	}

	///NewType creates a new reflection type of abi type given in t.
	std::shared_ptr<Type> NewType(std::string const& _t, std::string const& _internalType, ArgumentMarshalings components);

}

