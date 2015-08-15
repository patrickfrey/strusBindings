%inline %{
#include "objInitializers.hpp"
%}

%typemap(javapackage) std::vector<Rank>  "net.strus.api.RankVector"
%template(RankVector) std::vector<Rank>;

