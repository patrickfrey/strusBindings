%inline %{
#include "objInitializers.hpp"
%}

namespace std {
	%template(VecRankVector) vector<VecRank>;
}

%typemap(out) std::vector<VecRank>
{
	if (getVecRankVector( $result, $1))
	{
		SWIG_fail;
	}
}

