#if defined(SWIGPHP)
%typemap(in) const Tokenizer&	(Tokenizer temp)
{
	switch ((*$input)->type)
	{
		case IS_LONG:
			SWIG_exception( SWIG_RuntimeError, "unable to convert LONG to Tokenizer");
			break;
		case IS_STRING:
			temp.setName( std::string( Z_STRVAL_PP( $input)));
			break;
		case IS_DOUBLE:
			SWIG_exception( SWIG_RuntimeError, "unable to convert DOUBLE to Tokenizer");
			break;
		case IS_BOOL:
			SWIG_exception( SWIG_RuntimeError, "unable to convert BOOL to Tokenizer");
			break;
		case IS_NULL:
			break;
		case IS_ARRAY:
		{
			zval **data;
			HashTable *hash;
			HashPosition ptr;
			hash = Z_ARRVAL_PP($input);
			int argcnt = 0;
			for(
				zend_hash_internal_pointer_reset_ex(hash,&ptr);
				zend_hash_get_current_data_ex(hash,(void**)&data,&ptr) == SUCCESS;
				zend_hash_move_forward_ex(hash,&ptr),++argcnt)
			{
				zval val, *str;
				INIT_ZVAL( val);

				int is_str = (Z_TYPE_PP(data) == IS_STRING);
				if (is_str)
				{
					str = *data;
				}
				else
				{
					val = **data;
					zval_copy_ctor(&val);
					convert_to_string(&val);
					str = &val;
				}
				if (argcnt == 0)
				{
					temp.setName( Z_STRVAL_P(str));
				}
				else
				{
					temp.addArgument( Z_STRVAL_P(str));
				}
				if (!is_str)
				{
					zval_dtor(&val);
				}
			}
			break;
		}
		case IS_OBJECT:
			SWIG_exception( SWIG_RuntimeError, "unable to convert OBJECT to Tokenizer");
			break;
		case IS_RESOURCE:
			SWIG_exception( SWIG_RuntimeError, "unable to convert RESOURCE to Tokenizer");
			break;
		default: 
			SWIG_exception( SWIG_RuntimeError, "unable to convert unknown type to Tokenizer");
			break;
	}
	$1 = &temp;
}
#else
#error no typemaps defined for Tokenizer in this language
#endif


