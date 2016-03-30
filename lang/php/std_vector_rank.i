%inline %{
#include "objInitializers.hpp"
%}

namespace std {
	%template(RankVector) vector<Rank>;
}

%typemap(out) std::vector<Rank>
{
	if (getRankVector( $result, $1))
	{
		SWIG_fail;
	}
}

