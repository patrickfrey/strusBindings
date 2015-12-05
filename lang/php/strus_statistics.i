%inline %{
#include "objInitializers.hpp"
%}

%typemap(in) const GlobalStatistics&	(GlobalStatistics temp)
{
	GlobalStatistics* obj;
	if (SWIG_IsOK( SWIG_ConvertPtr( *$input, (void**)&obj, SWIGTYPE_p_GlobalStatistics, 0)))
	{
		$1 = obj;
	}
	else if (0!=initGlobalStatistics( temp, *$input))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}


%typemap(in) const TermStatistics&	(TermStatistics temp)
{
	TermStatistics* obj;
	if (SWIG_IsOK( SWIG_ConvertPtr( *$input, (void**)&obj, SWIGTYPE_p_TermStatistics, 0)))
	{
		$1 = obj;
	}
	else if (0!=initTermStatistics( temp, *$input))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}

