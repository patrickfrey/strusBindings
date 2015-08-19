%inline %{
#include "objInitializers.hpp"
%}

%typemap(in) const Normalizer&	(Normalizer temp)
{
	if (0!=initNormalizer( temp, $input))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}

%typemap(typecheck,match="in",precedence=SWIG_TYPECHECK_STRING) const Normalizer& {
	$1 = (PyString_Check( $input) || PySequence_Check( $input))?1:0;
}

%typemap(in) const std::vector<Normalizer>&	(std::vector<Normalizer> temp)
{
	if (0!=initNormalizerList( temp, $input))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}

%typemap(typecheck,match="in",precedence=SWIG_TYPECHECK_STRING) const std::vector<Normalizer>& {
	$1 = (PyString_Check( $input) || PySequence_Check( $input))?1:0;
}

