%inline %{
#include "objInitializers.hpp"
%}

namespace std {
	%template(RankVector) vector<Rank>;
}

%typemap(out) std::vector<Rank>
{
	resultobj = getRankVector( $1);
	if (!resultobj)
	{
		SWIG_fail;
	}
}

