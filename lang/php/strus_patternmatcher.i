%inline %{
#include "objInitializers.hpp"
%}

%typemap(in) const PatternMatcher&	(PatternMatcher temp)
{
	PatternMatcher* qe;
	if (SWIG_IsOK( SWIG_ConvertPtr( *$input, (void**)&qe, SWIGTYPE_p_PatternMatcher, 0)))
	{
		$1 = qe;
	}
	else if (0!=initPatternMatcher( temp, *$input))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}

