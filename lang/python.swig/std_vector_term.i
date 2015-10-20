%inline %{
#include "objInitializers.hpp"
%}

namespace std {
	%template(TermVector) vector<Term>;
}

%typemap(out) std::vector<Term>
{
	resultobj = getTermVector( $1);
	if (!resultobj)
	{
		SWIG_fail;
	}
}

