%inline %{
#include "objInitializers.hpp"
%}

namespace std {
	%template(RankVector) vector<Rank>;
}

%typemap(out) std::vector<Rank>
{
	if (getRankVector( return_value, $1))
	{
		SWIG_fail;
	}
}

