%include "objInitializers.hpp"

%typemap(in) const SummarizerConfig&	(SummarizerConfig temp)
{
	if (0!=initSummarizerConfig( &temp, (*$input)))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}
