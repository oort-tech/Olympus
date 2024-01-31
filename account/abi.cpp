#include "abi.hpp"
#include "utils.hpp"
#include "funcSelector.hpp"
#include <libdevcore/Exceptions.h>

namespace dev
{
	void from_json(const nlohmann::json& j, AbiMarshaling& p)
	{
		if (j.count("type"))
			j.at("type").get_to(p.Type);
		if (j.count("name"))
			j.at("name").get_to(p.Name);
		if (j.count("stateMutability"))
			j.at("stateMutability").get_to(p.StateMutability);
		if (j.count("constant"))
			j.at("constant").get_to(p.Constant);
		if (j.count("payable"))
			j.at("payable").get_to(p.Payable);
		if (j.count("anonymous"))
			j.at("anonymous").get_to(p.Anonymous);
		if (j.count("inputs"))
		{
			from_json(j.at("inputs"), p.Inputs);
		}
		if (j.count("outputs"))
		{
			from_json(j.at("outputs"), p.Outputs);
		}
	}

	ABI::ABI(AbiMarshalings const & fields)
	{
		for (auto field : fields)
		{
			if ("constructor" == field.Type)
			{
				Constructor = NewMethod("", "", FunctionType::Constructor, field.StateMutability, field.Constant, field.Payable, field.Inputs, Arguments());
			}
			else if ("function" == field.Type)
			{
				std::string name = ResolveNameConflict(field.Name, [this](std::string const& _name) { return Methods.count(_name); });
				Methods[name] = NewMethod(name, field.Name, FunctionType::Function, field.StateMutability, field.Constant, field.Payable, field.Inputs, field.Outputs);
			}
			else if ("fallback" == field.Type)
			{
				/// New introduced function type in v0.6.0, check more detail
				/// here https://solidity.readthedocs.io/en/v0.6.0/contracts.html#fallback-function
				if (HasFallback())
					BOOST_THROW_EXCEPTION(dev::FailedABI());
				Fallback = NewMethod("", "", FunctionType::Fallback, field.StateMutability, field.Constant, field.Payable, Arguments(), Arguments());
			}
			else if ("receive" == field.Type)
			{
				if (HasReceive())
					BOOST_THROW_EXCEPTION(dev::FailedABI());
				if (field.StateMutability != "payable")
					BOOST_THROW_EXCEPTION(dev::FailedABI());
				Receive = NewMethod("", "", FunctionType::Receive, field.StateMutability, field.Constant, field.Payable, Arguments(), Arguments());
			}
			else if ("event" == field.Type)///do it in the future
			{
			}
			else if ("error" == field.Type)///do it in the future
			{
			}
			else
				BOOST_THROW_EXCEPTION(dev::FailedABI());
		}
	}

	Arguments ABI::getArguments(std::string const & name, dev::bytes const & data)
	{
		/// since there can't be naming collisions with contracts and events,
		/// we need to decide whether we're calling a method or an event.
		/// but now only method
		if (data.size() % 32 != 0)
			BOOST_THROW_EXCEPTION(dev::FailedABI());///failed data
		if (!Methods.count(name))
			BOOST_THROW_EXCEPTION(dev::FailedABI());
		Method method = Methods[name];
		return method.Outputs;
	}

	ABI JSON(std::string reader)
	{
		nlohmann::json r = nlohmann::json::parse(reader);
		AbiMarshalings j = r.get<AbiMarshalings>();
		return ABI(j);
	}

	// revertSelector is a special function selector for revert reason unpacking.
	dev::FixedHash<4> revertSelector = selectorFromSignatureH32("Error(string)");

	bool UnpackRevert(dev::bytes const& data, std::string& result)
	{
		if (data.size() < 4)
		{
			result = "invalid data for unpacking";
			return false;
		}
		dev::FixedHash<4> _selector(bytesConstRef(&data).cropped(0, 4).toBytes());
		if (_selector != revertSelector)
		{
			result = "invalid data for unpacking";
			return false;
		}

		try
		{
			Arguments arg;
			arg.push_back(Argument("", NewType("string", "", ArgumentMarshalings()), false));
			arg.Unpack(bytesConstRef(&data).cropped(4).toBytes(), result);
			return true;
		}
		catch (dev::FailedABI& _e)
		{
			result = "invalid data for unpacking";
			return false;
		}
		catch (const std::exception&)
		{
			result = "invalid data for unpacking";
			return false;
		}
	}
	
}