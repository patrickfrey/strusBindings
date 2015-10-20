%inline %{
#include "objInitializers.hpp"
#include <iostream>
%}

%typemap(in) const Tokenizer&	(Tokenizer temp)
{
	if (0!=initTokenizer( temp, $input))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}

%typemap(typecheck,match="in",precedence=SWIG_TYPECHECK_STRING) const Tokenizer& {
	$1 = (PyString_Check( $input) || PySequence_Check( $input))?1:0;
}
