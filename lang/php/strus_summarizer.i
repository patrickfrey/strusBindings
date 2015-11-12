%inline %{
#include "objInitializers.hpp"
%}

%typemap(in) const SummarizerConfig&	(SummarizerConfig temp)
{
	SummarizerConfig* obj;
	if (SWIG_IsOK( SWIG_ConvertPtr( *$input, (void**)&obj, SWIGTYPE_p_SummarizerConfig, 0)))
	{
		$1 = obj;
	}
	else if (0!=initSummarizerConfig( temp, *$input))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}

