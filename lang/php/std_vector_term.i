%inline %{
#include "objInitializers.hpp"
%}

namespace std {
	%template(TermVector) vector<Term>;
}

%typemap(out) std::vector<Term>
{
	if (getTermVector( $result, $1))
	{
		SWIG_fail;
	}
}


