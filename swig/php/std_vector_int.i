%inline %{
#include "objInitializers.hpp"
%}

namespace std {
	%template(IntVector) vector<int>;
}

%typemap(in) std::vector<int>&	(std::vector<int> temp)
{
	if (0!=initIntVector( temp, *$input))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}
