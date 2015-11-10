%inline %{
#include "objInitializers.hpp"
%}

%typemap(in) const QueryExpression&	(QueryExpression temp)
{
	if (0!=initQueryExpression( temp, *$input))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}

