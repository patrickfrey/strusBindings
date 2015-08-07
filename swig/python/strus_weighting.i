%include "objInitializers.hpp"

%typemap(in) const WeightingConfig&	(SummarizerConfig temp)
{
	if (0!=initWeightingConfig( &temp, (*$input)))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}

