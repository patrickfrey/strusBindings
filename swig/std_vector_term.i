namespace std {
	%template(TermVector) vector<Term>;
}

#if defined(SWIGPHP)
%typemap(out) std::vector<Term>
{
	array_init( return_value);
	std::vector<Term>::const_iterator ti = $input.begin(), te = $input.end();
	for (; ti != te; ++ti)
	{
		zval* term;
		MAKE_STD_ZVAL( term);
		object_init( term);
		add_property_string( term, "type", reinterpret_cast<char*>(ti->type().c_str()), 1);
		add_property_string( term, "value", reinterpret_cast<char*>(ti->value().c_str()), 1);
		add_property_long( term, "position", ti->position());
		add_next_index_zval( return_value, term);
	}
}
#else
#error no typemaps defined for std::vector<Term> in this language
#endif


