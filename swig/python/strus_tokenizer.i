%include "objInitializers.hpp"

%typemap(in) const Tokenizer&	(Tokenizer temp)
{
	if (0!=initTokenizer( temp, *$input))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}

