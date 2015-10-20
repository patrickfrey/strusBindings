%inline %{
#include "objInitializers.hpp"
%}

%typemap(in) const SummarizerConfig&	(SummarizerConfig temp)
{
	if (0!=initSummarizerConfig( temp, $input))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}

%typemap(typecheck,match="in",precedence=SWIG_TYPECHECK_STRING) const SummarizerConfig& {
	$1 = (PyString_Check( $input) || PySequence_Check( $input))?1:0;
}
