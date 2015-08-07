namespace std {
	%template(TermVector) vector<Term>;
}

%typemap(out) std::vector<Rank>
{
	return_value = getTermVector( $input);
	if (!return_value)
	{
		SWIG_fail;
	}
}

