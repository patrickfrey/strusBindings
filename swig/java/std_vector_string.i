%inline %{
#include "objInitializers.hpp"
%}

%template(StringVector) std::vector<std::string>;
%typemap(in) const std::vector<std::string>&	(std::vector<std::string> temp)
{
	if (0!=strus::initStringVector( temp, jenv, $input))
	{
		return $null;
	}
	else
	{
		$1 = &temp;
	}
}

