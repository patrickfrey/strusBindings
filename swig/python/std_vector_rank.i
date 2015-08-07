namespace std {
	%template(RankVector) vector<Rank>;
}

%typemap(out) std::vector<Rank>
{
	return_value = getRankVector( $input);
	if (!return_value)
	{
		SWIG_fail;
	}
}

