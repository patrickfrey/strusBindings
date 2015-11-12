%inline %{
#include "objInitializers.hpp"
%}

%typemap(in) const WeightingConfig&	(WeightingConfig temp)
{
	WeightingConfig* obj;
	if (SWIG_IsOK( SWIG_ConvertPtr( *$input, (void**)&obj, SWIGTYPE_p_WeightingConfig, 0)))
	{
		$1 = obj;
	}
	else if (0!=initWeightingConfig( temp, *$input))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}

