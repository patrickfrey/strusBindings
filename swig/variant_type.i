
#if defined(SWIGPHP)
%typemap(in) const Variant&
{
	switch ((*$input)->type)
	{
		case IS_LONG: $1->assign((int)Z_LVAL_PP( $input)); break;
		case IS_STRING: $1->assign(Z_STRVAL_PP( $input)); break;
		case IS_DOUBLE: $1->assign(Z_DVAL_PP( $input)); break;
		case IS_BOOL: $1->assign( (int)Z_BVAL_PP( $input)); break;
		case IS_NULL: $1->init(); break;
		case IS_ARRAY: throw std::runtime_error("unable to convert ARRAY to strus Variant type");
		case IS_OBJECT: throw std::runtime_error("unable to convert OBJECT to strus Variant type");
		case IS_RESOURCE: throw std::runtime_error("unable to convert RESOURCE to strus Variant type");
		default: throw std::runtime_error("unable to convert unknown type to strus Variant type");
	}
}
#else
#error no "in" typemap defined
#endif

