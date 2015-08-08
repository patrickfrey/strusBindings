%inline %{
#include "objInitializers.hpp"
%}

%typemap(in) const WeightingConfig&	(WeightingConfig temp)
{
	if (0!=initWeightingConfig( temp, jenv, $input))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}

