%inline %{
#include "objInitializers.hpp"
%}

%typemap(in) const FunctionVariableConfig&	(FunctionVariableConfig temp)
{
	FunctionVariableConfig* obj;
	if (SWIG_IsOK( SWIG_ConvertPtr( *$input, (void**)&obj, SWIGTYPE_p_FunctionVariableConfig, 0)))
	{
		$1 = obj;
	}
	else if (0!=initFunctionVariableConfig( temp, *$input))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}

