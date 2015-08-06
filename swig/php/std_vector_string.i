namespace std {
	%template(StringVector) vector<string>;
}

%typemap(in) std::vector<std::string>&	(std::vector<std::string> ar)
{
	switch ((*$input)->type)
	{
		case IS_LONG:
			SWIG_exception( SWIG_RuntimeError, "unable to convert LONG to std::vector<std::string>");
			break;
		case IS_STRING:
			ar.push_back( Z_STRVAL_PP( $input));
			break;
		case IS_DOUBLE:
			SWIG_exception( SWIG_RuntimeError, "unable to convert DOUBLE to std::vector<std::string>");
			break;
		case IS_BOOL:
			SWIG_exception( SWIG_RuntimeError, "unable to convert BOOL to std::vector<std::string>");
			break;
		case IS_NULL:
			break;
		case IS_ARRAY:
		{
			zval **data;
			HashTable *hash;
			HashPosition ptr;
			hash = Z_ARRVAL_PP($input);
			for(
				zend_hash_internal_pointer_reset_ex(hash,&ptr);
				zend_hash_get_current_data_ex(hash,(void**)&data,&ptr) == SUCCESS;
				zend_hash_move_forward_ex(hash,&ptr))
			{
				zval temp, *str;
				INIT_ZVAL( temp);

				int is_str = 1;
				if (Z_TYPE_PP(data) != IS_STRING)
				{
					temp = **data;
					zval_copy_ctor(&temp);
					convert_to_string(&temp);
					str = &temp;
					is_str = 0;
				}
				else
				{
					str = *data;
				}
				ar.push_back( std::string( Z_STRVAL_P(str)));
				if (!is_str)
				{
					zval_dtor(&temp);
				}
			}
			break;
		}
		case IS_OBJECT:
			SWIG_exception( SWIG_RuntimeError, "unable to convert OBJECT to std::vector<std::string>");
			break;
		case IS_RESOURCE:
			SWIG_exception( SWIG_RuntimeError, "unable to convert RESOURCE to std::vector<std::string>");
			break;
		default: 
			SWIG_exception( SWIG_RuntimeError, "unable to convert unknown type to std::vector<std::string>");
			break;
	}
	$1 = &ar;
}

