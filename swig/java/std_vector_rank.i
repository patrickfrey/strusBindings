%inline %{
#include "objInitializers.hpp"
%}

%typemap(javapackage) std::vector<Rank>  "net.strus.api.RankVector"
%template(RankVector) std::vector<Rank>;

//%typemap(out) std::vector<Rank>
//{
//	resultobj = getRankVector( jenv, $1);
//	if (!resultobj)
//	{
//		SWIG_fail;
//	}
//}

