%inline %{
#include "objInitializers.hpp"
%}

%typemap(in) const Variant& (Variant temp)
{
	if (0!=initVariant( temp, $input))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}

%typemap(typecheck,match="in",precedence=SWIG_TYPECHECK_STRING_ARRAY) const Variant& {
	$1 = (PyLong_Check( $input) || PyInt_Check( $input) || PyFloat_Check( $input) || PyString_Check( $input))?1:0;
}

