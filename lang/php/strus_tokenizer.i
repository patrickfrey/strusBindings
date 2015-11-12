%inline %{
#include "objInitializers.hpp"
%}

%typemap(in) const Tokenizer&	(Tokenizer temp)
{
	Tokenizer* obj;
	if (SWIG_IsOK( SWIG_ConvertPtr( *$input, (void**)&obj, SWIGTYPE_p_Tokenizer, 0)))
	{
		$1 = obj;
	}
	else if (0!=initTokenizer( temp, *$input))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}

