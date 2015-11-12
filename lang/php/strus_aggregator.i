%inline %{
#include "objInitializers.hpp"
%}

%typemap(in) const Aggregator&	(Aggregator temp)
{
	Aggregator* obj;
	if (SWIG_IsOK( SWIG_ConvertPtr( *$input, (void**)&obj, SWIGTYPE_p_Aggregator, 0)))
	{
		$1 = obj;
	}
	else if (0!=initAggregator( temp, *$input))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}


