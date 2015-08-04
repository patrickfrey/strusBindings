#if defined(SWIGPHP)
%typemap(in) const Normalizer&	(Normalizer temp)
{
	switch ((*$input)->type)
	{
		case IS_LONG:
			SWIG_exception( SWIG_RuntimeError, "unable to convert LONG to Normalizer");
			break;
		case IS_STRING:
			temp.setName( std::string( Z_STRVAL_PP( $input)));
			break;
		case IS_DOUBLE:
			SWIG_exception( SWIG_RuntimeError, "unable to convert DOUBLE to Normalizer");
			break;
		case IS_BOOL:
			SWIG_exception( SWIG_RuntimeError, "unable to convert BOOL to Normalizer");
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
			SWIG_exception( SWIG_RuntimeError, "unable to convert OBJECT to Normalizer");
			break;
		case IS_RESOURCE:
			SWIG_exception( SWIG_RuntimeError, "unable to convert RESOURCE to Normalizer");
			break;
		default: 
			SWIG_exception( SWIG_RuntimeError, "unable to convert unknown type to Normalizer");
			break;
	}
	$1 = &temp;
}

%typemap(in) const std::vector<Normalizer>&	(std::vector<Normalizer> temp)
{
	switch ((*$input)->type)
	{
		case IS_LONG:
			SWIG_exception( SWIG_RuntimeError, "unable to convert LONG to std::vector<Normalizer>");
			break;
		case IS_STRING:
			temp.push_back( Normalizer( std::string( Z_STRVAL_PP( $input))));
			break;
		case IS_DOUBLE:
			SWIG_exception( SWIG_RuntimeError, "unable to convert DOUBLE to std::vector<Normalizer>");
			break;
		case IS_BOOL:
			SWIG_exception( SWIG_RuntimeError, "unable to convert BOOL to std::vector<Normalizer>");
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
					temp.push_back( Normalizer( Z_STRVAL_P(str)));
					zval_dtor(&val);
				}
				else if (Z_TYPE_PP(data) == IS_ARRAY)
				{
					temp.push_back( Normalizer());

					zval **e_data;
					HashTable *e_hash;
					HashPosition e_ptr;
					e_hash = Z_ARRVAL_PP(data);
					int e_argcnt = 0;
					for(
						zend_hash_internal_pointer_reset_ex(e_hash,&e_ptr);
						zend_hash_get_current_data_ex(hash,(void**)&e_data,&e_ptr) == SUCCESS;
						zend_hash_move_forward_ex(e_hash,&e_ptr),++e_argcnt)
					{
						zval e_val, *e_str;
						INIT_ZVAL( e_val);
						int e_is_str = (Z_TYPE_PP(e_data) == IS_STRING);
						if (e_is_str)
						{
							e_val = **e_data;
							zval_copy_ctor(&e_val);
							convert_to_string(&e_val);
							e_str = &e_val;
						}
						else
						{
							e_str = *e_data;
						}
						if (e_argcnt == 0)
						{
							temp.back().setName( Z_STRVAL_P(e_str));
						}
						else
						{
							temp.back().addArgument( Z_STRVAL_P(e_str));
						}
						if (!e_is_str)
						{
							zval_dtor(&e_val);
						}
					}
				}
				else
				{
					val = **data;
					zval_copy_ctor(&val);
					convert_to_string(&val);
					str = &val;
					temp.push_back( Normalizer( Z_STRVAL_P(str)));
				}				
			}
			break;
		}
		case IS_OBJECT:
			SWIG_exception( SWIG_RuntimeError, "unable to convert OBJECT to std::vector<Normalizer>");
			break;
		case IS_RESOURCE:
			SWIG_exception( SWIG_RuntimeError, "unable to convert RESOURCE to std::vector<Normalizer>");
			break;
		default: 
			SWIG_exception( SWIG_RuntimeError, "unable to convert unknown type to std::vector<Normalizer>");
			break;
	}
	$1 = &temp;
}

#else
#error no typemaps defined for Normalizer in this language
#endif


