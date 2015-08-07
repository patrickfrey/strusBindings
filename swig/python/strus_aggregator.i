%typemap(in) const Aggregator&	(Aggregator temp)
{
	if (0!=initAggregator( &temp, (*$input)))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}


