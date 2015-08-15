%inline %{
#include "objInitializers.hpp"
%}

%typemap(javapackage) std::vector<TermVector>  "net.strus.api.TermVector"
%template(NormalizerVector) std::vector<Normalizer>;
%typemap(out) std::vector<Normalizer>
{
	resultobj = getNormalizerVector( $1);
	if (!resultobj)
	{
		return $null;
	}
}
%typemap(in) const std::vector<Normalizer>&	(std::vector<Normalizer> temp)
{
	if (0!=initNormalizerVector( temp, jenv, $input))
	{
		return $null;
	}
	else
	{
		$1 = &temp;
	}
}


