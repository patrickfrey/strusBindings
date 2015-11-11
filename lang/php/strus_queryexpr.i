%inline %{
#include "objInitializers.hpp"
%}

%typemap(in) const QueryExpression&	(QueryExpression temp)
{
	QueryExpression* qe;
	if (SWIG_IsOK( SWIG_ConvertPtr( *$input, (void**)&qe, SWIGTYPE_p_QueryExpression, 0)))
	{
		$1 = qe;
	}
	else if (0!=initQueryExpression( temp, *$input))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}

