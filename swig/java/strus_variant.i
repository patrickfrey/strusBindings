%inline %{
#include "objInitializers.hpp"
%}

%typemap(in) const Variant& value (Variant temp) {
	temp.assign( **(Variant**)&$input);
	$1 = &temp;
}

