%inline %{
#include "objInitializers.hpp"
%}

%typemap(in) const Variant& (Variant temp)
{
	if (0!=initVariant( temp, jenv, $input))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}

