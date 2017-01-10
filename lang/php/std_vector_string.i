%inline %{
#include "objInitializers.hpp"
%}

namespace std {
	%template(StringVector) vector<string>;
}

%typemap(in) std::vector<std::string>&	(std::vector<std::string> temp)
{
	if (0!=initStringVector( temp, *$input))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}

%typemap(out) std::vector<std::string>
{
	if (getStringVector( $result, $1))
	{
		SWIG_fail;
	}
}

