%inline %{
#include "objInitializers.hpp"
%}

%typemap(in) const Normalizer&	(Normalizer temp)
{
	Normalizer* obj;
	if (SWIG_IsOK( SWIG_ConvertPtr( *$input, (void**)&obj, SWIGTYPE_p_Normalizer, 0)))
	{
		$1 = obj;
	}
	else if (0!=initNormalizer( temp, *$input))
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
	NormalizerVector* obj;
	if (SWIG_IsOK( SWIG_ConvertPtr( *$input, (void**)&obj, SWIGTYPE_p_std__vectorT_Normalizer_t, 0)))
	{
		$1 = obj;
	}
	else if (0!=initNormalizerList( temp, *$input))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}



