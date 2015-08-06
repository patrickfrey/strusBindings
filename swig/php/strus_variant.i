%typemap(in) const Variant& (Variant temp)
{
	switch ((*$input)->type)
	{
		case IS_LONG: temp.assign((int)Z_LVAL_PP( $input)); break;
		case IS_STRING: temp.assign(Z_STRVAL_PP( $input)); break;
		case IS_DOUBLE: temp.assign(Z_DVAL_PP( $input)); break;
		case IS_BOOL: temp.assign( (int)Z_BVAL_PP( $input)); break;
		case IS_NULL: temp.init(); break;
		case IS_ARRAY: SWIG_exception( SWIG_RuntimeError, "unable to convert ARRAY to strus Variant type");
		case IS_OBJECT: SWIG_exception( SWIG_RuntimeError, "unable to convert OBJECT to strus Variant type");
		case IS_RESOURCE: SWIG_exception( SWIG_RuntimeError, "unable to convert RESOURCE to strus Variant type");
		default: SWIG_exception( SWIG_RuntimeError, "unable to convert unknown type to strus Variant type");
	}
	$1 = &temp;
}

