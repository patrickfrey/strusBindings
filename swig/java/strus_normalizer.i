%inline %{
#include "objInitializers.hpp"
%}

%typemap(in) const Normalizer&	(Normalizer temp)
{
	if (0!=initNormalizer( temp, jenv, $input))
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
	if (0!=initNormalizerList( temp, jenv, $input))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}


