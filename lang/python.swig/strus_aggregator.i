%inline %{
#include "objInitializers.hpp"
%}

%typemap(in) const Aggregator&	(Aggregator temp)
{
	if (0!=initAggregator( temp, $input))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}

%typemap(typecheck,match="in",precedence=SWIG_TYPECHECK_STRING_ARRAY) const Aggregator& {
	$1 = (PyString_Check( $input) || PySequence_Check( $input))?1:0;
}
