%typemap(in) const WeightingConfig&	(WeightingConfig temp)
{
	switch ((*$input)->type)
	{
		case IS_LONG:
			SWIG_exception( SWIG_RuntimeError, "unable to convert LONG to WeightingConfig");
			break;
		case IS_STRING:
			SWIG_exception( SWIG_RuntimeError, "unable to convert STRING to WeightingConfig");
			break;
		case IS_DOUBLE:
			SWIG_exception( SWIG_RuntimeError, "unable to convert DOUBLE to WeightingConfig");
			break;
		case IS_BOOL:
			SWIG_exception( SWIG_RuntimeError, "unable to convert BOOL to WeightingConfig");
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
				zval elem;
				char *name = 0;
				unsigned int name_len = 0;// length of string including 0 byte !
				unsigned long index;
				if (!zend_hash_get_current_key_ex( hash, &name, &name_len, &index, 0, &ptr) == HASH_KEY_IS_STRING)
				{
					SWIG_exception( SWIG_RuntimeError, "illegal key of sumarizer element (not a string)");
				}
				if (name && name[0] == '.')
				{
					try
					{
						temp.defineFeature( std::string( name+1, name_len-2), std::string( Z_STRVAL_P(*data)));
					}
					catch (...)
					{
						SWIG_exception( SWIG_RuntimeError, "memory allocation error");
					}
				}
				else
				{
					try
					{
						switch (Z_TYPE_PP(data))
						{
							case IS_LONG:
								temp.defineParameter( std::string( name, name_len-1), Variant( (int)Z_LVAL_P( *data)));
								break;
							case IS_STRING:
								temp.defineParameter( std::string( name, name_len-1), Variant( (char*)Z_STRVAL_P( *data)));
								break;
							case IS_DOUBLE:
								temp.defineParameter( std::string( name, name_len-1), Variant( (double)Z_DVAL_P( *data)));
								break;
							case IS_BOOL:
								temp.defineParameter( std::string( name, name_len-1), Variant( (bool)Z_BVAL_P( *data)));
								break;
						}
					}
					catch (...)
					{
						SWIG_exception( SWIG_RuntimeError, "memory allocation error");
					}
				}
			}
			break;
		}
		case IS_OBJECT:
			SWIG_exception( SWIG_RuntimeError, "unable to convert OBJECT to WeightingConfig");
			break;
		case IS_RESOURCE:
			SWIG_exception( SWIG_RuntimeError, "unable to convert RESOURCE to WeightingConfig");
			break;
		default: 
			SWIG_exception( SWIG_RuntimeError, "unable to convert unknown type to WeightingConfig");
			break;
	}
	$1 = &temp;
}

