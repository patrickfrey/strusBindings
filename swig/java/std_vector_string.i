%inline %{
#include "objInitializers.hpp"
%}

%template(StringVector) std::vector<std::string>;
%typemap(out) std::vector<std::string>
{
	resultobj = getStringVector( $1);
	if (!resultobj)
	{
		return $null;
	}
}
%typemap(in) const std::vector<std::string>&	(std::vector<std::string> temp)
{
	if (0!=initStringVector( temp, jenv, $input))
	{
		return $null;
	}
	else
	{
		$1 = &temp;
	}
}

