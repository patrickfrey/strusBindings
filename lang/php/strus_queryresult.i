%inline %{
#include "objInitializers.hpp"
%}

%typemap(out) QueryResult
{
	if (getQueryResult( return_value, $1))
	{
		SWIG_fail;
	}
}

