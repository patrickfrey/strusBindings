%inline %{
#include "objInitializers.hpp"
%}

namespace std {
	%template(FloatVector) vector<double>;
}

%typemap(in) std::vector<double>&	(std::vector<double> temp)
{
	if (0!=initFloatVector( temp, *$input))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}

%typemap(out) std::vector<double>
{
	if (getFloatVector( $result, $1))
	{
		SWIG_fail;
	}
}



