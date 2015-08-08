%inline %{
#include "objInitializers.hpp"
%}

%typemap(in) const SummarizerConfig&	(SummarizerConfig temp)
{
	if (0!=initSummarizerConfig( temp, jenv, $input))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}

