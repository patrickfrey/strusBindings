%inline %{
#include "objInitializers.hpp"
%}

%rename(TokenNormalizer) Normalizer;

%typemap(in) const Normalizer&	(Normalizer temp)
{
	if (0!=initNormalizer( temp, *$input))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}

%typemap(in) const std::vector<Normalizer>&	(std::vector<Normalizer> temp)
{
	if (0!=initNormalizerList( temp, *$input))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}



