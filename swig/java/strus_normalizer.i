%inline %{
#include "objInitializers.hpp"
%}

%typemap(javapackage) std::vector<TermVector>  "net.strus.api.TermVector"
%template(NormalizerVector) std::vector<Normalizer>;


