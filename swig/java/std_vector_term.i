%inline %{
#include "objInitializers.hpp"
%}
%typemap(javapackage) std::vector<Term>  "net.strus.api.TermVector"
%template(TermVector) std::vector<Term>;
