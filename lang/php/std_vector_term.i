%inline %{
#include "objInitializers.hpp"
%}

namespace std {
	%template(TermVector) vector<Term>;
}

%typemap(out) std::vector<Term>
{
	if (getTermVector( return_value, $1))
	{
		SWIG_fail;
	}
}


