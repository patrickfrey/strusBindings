namespace std {
	%template(StringVector) vector<string>;
}

%typemap(in) std::vector<std::string>&	(std::vector<std::string> ar)
{
	if (0!=initStringVector( temp, *$input))
	{
		SWIG_fail;
	}
	else
	{
		$1 = &temp;
	}
}
