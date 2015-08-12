%inline %{
#include "objInitializers.hpp"
%}

//%typemap(in) const Aggregator&	(Aggregator temp)
//{
//	if (0!=initAggregator( temp, jenv, $input))
//	{
//		SWIG_fail;
//	}
//	else
//	{
//		$1 = &temp;
//	}
//}


