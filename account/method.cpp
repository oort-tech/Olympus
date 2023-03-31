#include "method.hpp"
#include "funcSelector.hpp"

namespace dev
{
	Method NewMethod(std::string const & _name, std::string const & _RawName, FunctionType const & _Type, std::string const & _StateMutability, bool const & _Constant, bool const & _Payable, Arguments const & _Inputs, Arguments const & _Outputs)
	{
		std::vector<std::string> types(_Inputs.size()), inputNames(_Inputs.size()), outputNames(_Outputs.size());
		for (size_t i = 0; i < _Inputs.size(); i++)
		{
			inputNames[i] = boost::str(boost::format("%1% %2%") % _Inputs[i].Type->stringKind % _Inputs[i].Name); //todo ???????????????????????????
			types[i] = _Inputs[i].Type->stringKind;
		}
		for (size_t i = 0; i < _Outputs.size(); i++)
		{
			outputNames[i] = _Outputs[i].Type->stringKind; //todo
			if (_Outputs[i].Name.length()) /// name valid
				outputNames[i] = outputNames[i] + " " + _Outputs[i].Name;
		}

		std::string sig;
		dev::bytes id;
		/// calculate the signature and method id. Note only function
		/// has meaningful signature and id.
		if (_Type == FunctionType::Function)
		{
			sig = boost::str(boost::format("%1%(%2%)") % _RawName % boost::algorithm::join(types, ","));
			sig = _RawName + "(" + boost::algorithm::join(types, ",") + ")";
			id = selectorFromSignatureH32(sig).asBytes();
		}
		/// Extract meaningful state mutability of solidity method.
		/// If it's default value, never print it.
		std::string state = _StateMutability;
		if (state == "nonpayable")
			state = "";
		if (state != "")
			state = state + " ";
		std::string identity = "function " + _RawName;
		if (_Type == Fallback)
			identity = "fallback";
		else if (_Type == Receive)
			identity = "receive";
		else if (_Type == Constructor)
			identity = "constructor";
		std::string str = boost::str(boost::format("%1%(%2%) %3%returns(%4%)") % identity % boost::algorithm::join(inputNames, ", ") % state % boost::algorithm::join(outputNames, ", "));
		return Method(_name, 
			_RawName, 
			_Type, 
			_StateMutability, 
			_Constant, 
			_Payable, 
			_Inputs, 
			_Outputs, 
			str, 
			sig, 
			id
		);
	}

}