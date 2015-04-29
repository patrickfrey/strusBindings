
#if defined(SWIGPHP)
%typemap(in) const Variant& (Variant temp)
{
	switch ((*$input)->type)
	{
		case IS_LONG: temp.assign((int)Z_LVAL_PP( $input)); break;
		case IS_STRING: temp.assign(Z_STRVAL_PP( $input)); break;
		case IS_DOUBLE: temp.assign(Z_DVAL_PP( $input)); break;
		case IS_BOOL: temp.assign( (int)Z_BVAL_PP( $input)); break;
		case IS_NULL: temp.init(); break;
		case IS_ARRAY: throw std::runtime_error("unable to convert ARRAY to strus Variant type");
		case IS_OBJECT: throw std::runtime_error("unable to convert OBJECT to strus Variant type");
		case IS_RESOURCE: throw std::runtime_error("unable to convert RESOURCE to strus Variant type");
		default: throw std::runtime_error("unable to convert unknown type to strus Variant type");
	}
	$1 = &temp;
}
#else
#error no typemaps defined for the strus variant type in this language
#endif

