#include "argument.hpp"
#include "funcSelector.hpp"

namespace dev
{
	void from_json(const nlohmann::json& j, Arguments& p)
	{
		ArgumentMarshalings args = j.get<ArgumentMarshalings>();
		for (auto arg : args)
		{
			auto ty = NewType(arg.Type, arg.InternalType, arg.Components);
			Argument ar(arg.Name, ty, arg.Indexed);
			p.push_back(ar);
		}
		
	}
	std::vector<Argument> Arguments::NonIndexed()
	{
		std::vector<Argument> ret;
		for (Argument arg : abiArgs)
		{
			if (!arg.Indexed)
				ret.push_back(arg);
		}
		return ret;
	}
}