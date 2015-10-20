%inline %{
#include "objInitializers.hpp"
%}

%typemap(in) const WeightingConfig&	(WeightingConfig temp)
{
	if (0!=initWeightingConfig( temp, $input))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}

%typemap(typecheck,match="in",precedence=SWIG_TYPECHECK_STRING) const WeightingConfig& {
	$1 = (PyString_Check( $input) || PySequence_Check( $input))?1:0;
}
