%inline %{
#include "objInitializers.hpp"
%}

%typemap(in) std::string&	(std::string temp)
{
	if (0!=initString( temp, $input))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}

