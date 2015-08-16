%inline %{
#include "objInitializers.hpp"
%}

%typemap(javapackage) std::vector<Rank>  "net.strus.api.RankVector"
%template(RankVector) std::vector<Rank>;

%typemap(javapackage) std::vector<RankAttribute>  "net.strus.api.RankAttributeVector"
%template(RankAttributeVector) std::vector<RankAttribute>;

