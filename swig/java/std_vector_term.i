%inline %{
#include "objInitializers.hpp"
%}

%typemap(javapackage) std::vector<Term>  "net.strus.api.TermVector"
%template(TermVector) std::vector<Term>;

//%typemap(out) std::vector<Term>
//{
//	resultobj = getTermVector( jenv, $1);
//	if (!resultobj)
//	{
//		SWIG_fail;
//	}
//}

