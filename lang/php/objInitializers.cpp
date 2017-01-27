/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "objInitializers.hpp"
#include <zend.h>
#include <zend_API.h>
#include <zend_exceptions.h>
#include <limits>
#include <iostream>
#include <sstream>
#include <boost/algorithm/string.hpp>

#define THROW_EXCEPTION( MSG) zend_throw_exception( NULL, const_cast<char*>( MSG), 0 TSRMLS_CC)

static bool caseInsensitiveEquals( const std::string& val1, const std::string& val2)
{
	return boost::algorithm::iequals( val1, val2);
}

int initVariant( Variant& result, zval* obj)
{
	switch (obj->type)
	{
		case IS_LONG: result.assignInt( Z_LVAL_P( obj)); return 0;
		case IS_STRING: result.assignText( Z_STRVAL_P( obj)); return 0;
		case IS_DOUBLE: result.assignFloat( Z_DVAL_P( obj)); return 0;
		case IS_BOOL: result.assignInt( Z_BVAL_P( obj)); return 0;
		case IS_NULL: result.init(); return 0;
		case IS_ARRAY: THROW_EXCEPTION( "unable to convert ARRAY to strus Variant type"); return -1;
		case IS_OBJECT: THROW_EXCEPTION( "unable to convert OBJECT to strus Variant type"); return -1;
		case IS_RESOURCE: THROW_EXCEPTION( "unable to convert RESOURCE to strus Variant type"); return -1;
	}
	THROW_EXCEPTION( "unable to convert unknown type to strus Variant type");
	return -1;
}

static void dumpObject_( std::ostream& out, zval* obj, int indent)
{
	switch (obj->type)
	{
		case IS_LONG:
			out << (long)Z_LVAL_P( obj);
			break;
		case IS_STRING:
			out << "'" << std::string( Z_STRVAL_P( obj)) << "'";
			break;
		case IS_DOUBLE:
			out << (double)Z_DVAL_P( obj);
			break;
		case IS_BOOL:
			out << (Z_BVAL_P( obj)?"True":"False");
			break;
		case IS_NULL:
			out << "NULL";
			break;
		case IS_ARRAY:
		{
			zval **data;
			HashTable *hash;
			HashPosition ptr;
			hash = Z_ARRVAL_P( obj);
			int argcnt = 0;
			for(
				zend_hash_internal_pointer_reset_ex(hash,&ptr);
				zend_hash_get_current_data_ex(hash,(void**)&data,&ptr) == SUCCESS;
				zend_hash_move_forward_ex(hash,&ptr),++argcnt)
			{
				if (argcnt) out << std::endl << std::string( indent, '\t');
				dumpObject_( out, *data, indent+1);
			}
			break;
		}
		case IS_OBJECT:
		{
			HashTable *objht = Z_OBJPROP_P( obj);
			char *key;
			unsigned int keysize;
			zval **data;
			HashPosition pos;

			unsigned int nofelements = zend_hash_num_elements( objht);
			zend_hash_internal_pointer_reset_ex(objht, &pos);
			for (unsigned int elemidx=0; elemidx<nofelements; zend_hash_move_forward_ex(objht, &pos),++elemidx)
			{
				unsigned long idx;
				int keytype = zend_hash_get_current_key_ex( objht, &key, &keysize, &idx, 0, &pos);
				if (keytype == HASH_KEY_NON_EXISTANT) continue;

				if (zend_hash_get_current_data_ex(objht, (void **) &data, &pos) == SUCCESS)
				{
					if (keytype == HASH_KEY_IS_STRING)
					{
						if (elemidx) out << std::endl << std::string( indent, '\t');
						out << "[" << std::string(key,keysize-1) << "] ";
						dumpObject_( out, *data, indent+1);
					}
				}
			}
			break;
		}
		case IS_RESOURCE:
			out << "<resource>";
			break;
		default:
			out << "<unknown>";
			break;
	}
}

std::string dumpObject( zval* obj)
{
	std::ostringstream out;
	dumpObject_( out, obj, 0);
	return out.str();
}

template <class Object>
static int initFunctionObject( Object& result, zval* obj)
{
	int error = 0;
	switch (obj->type)
	{
		case IS_LONG:
			THROW_EXCEPTION( "unable to convert LONG to function object");
			error = -1;
			break;
		case IS_STRING:
			try
			{
				result.setName( std::string( Z_STRVAL_P( obj)));
			}
			catch (...)
			{
				THROW_EXCEPTION( "memory allocation error");
				error = -1;
			}
			break;
		case IS_DOUBLE:
			THROW_EXCEPTION( "unable to convert DOUBLE to function object");
			error = -1;
			break;
		case IS_BOOL:
			THROW_EXCEPTION( "unable to convert BOOL to function object");
			error = -1;
			break;
		case IS_NULL:
			break;
		case IS_ARRAY:
		{
			zval **data;
			HashTable *hash;
			HashPosition ptr;
			hash = Z_ARRVAL_P( obj);
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
				try
				{
					if (argcnt == 0)
					{
						result.setName( Z_STRVAL_P( str));
					}
					else
					{
						result.addArgument( Z_STRVAL_P( str));
					}
				}
				catch (...)
				{
					THROW_EXCEPTION( "memory allocation error");
					error = -1;
				}
				if (!is_str)
				{
					zval_dtor(&val);
				}
			}
			break;
		}
		case IS_OBJECT:
			THROW_EXCEPTION( "unable to convert OBJECT to function object");
			error = -1;
			break;
		case IS_RESOURCE:
			THROW_EXCEPTION( "unable to convert RESOURCE to function object");
			error = -1;
			break;
		default: 
			THROW_EXCEPTION( "unable to convert unknown type to function object");
			error = -1;
			break;
	}
	return error;
}

int initTokenizer( Tokenizer& result, zval* obj)
{
	return initFunctionObject( result, obj);
}

int initNormalizer( Normalizer& result, zval* obj)
{
	return initFunctionObject( result, obj);
}

int initNormalizerList( std::vector<Normalizer>& result, zval* obj)
{
	int error = 0;
	switch (obj->type)
	{
		case IS_LONG:
			THROW_EXCEPTION( "unable to convert LONG to normalizer list ");
			error = -1;
			break;
		case IS_STRING:
			try
			{
				result.push_back( Normalizer( std::string( Z_STRVAL_P( obj))));
			}
			catch (...)
			{
				THROW_EXCEPTION( "memory allocation error");
				error = -1;
			}
			break;
		case IS_DOUBLE:
			THROW_EXCEPTION( "unable to convert DOUBLE to normalizer list ");
			error = -1;
			break;
		case IS_BOOL:
			THROW_EXCEPTION( "unable to convert BOOL to normalizer list ");
			error = -1;
			break;
		case IS_OBJECT:
			THROW_EXCEPTION( "unable to convert OBJECT to normalizer list ");
			error = -1;
			break;
		case IS_RESOURCE:
			THROW_EXCEPTION( "unable to convert RESOURCE to normalizer list ");
			error = -1;
			break;
		case IS_NULL:
			break;
		case IS_ARRAY:
		{
			zval **data;
			HashTable *hash;
			HashPosition ptr;
			hash = Z_ARRVAL_P( obj);
			int argcnt = 0;
			for(
				zend_hash_internal_pointer_reset_ex(hash,&ptr);
				zend_hash_get_current_data_ex(hash,(void**)&data,&ptr) == SUCCESS;
				zend_hash_move_forward_ex(hash,&ptr),++argcnt)
			{
				try
				{
					result.push_back( Normalizer());
					if (0!=initNormalizer( result.back(), *data))
					{
						error = -1;
						break;
					}
				}
				catch (...)
				{
					THROW_EXCEPTION( "memory allocation error");
					error = -1;
					break;
				}
			}
			break;
		}
		default: 
			THROW_EXCEPTION( "unable to convert unknown type to normalizer list ");
			error = -1;
			break;
	}
	return error;
}

int initAggregator( Aggregator& result, zval* obj)
{
	return initFunctionObject( result, obj);
}

static int initFunctionVariable( FunctionVariableConfig& result, const char* key, std::size_t keylen, zval* valueitem)
{
	int error = 0;
	try
	{
		switch (Z_TYPE_P(valueitem))
		{
			case IS_LONG:
				result.defineVariable( std::string( key, keylen), (double)Z_LVAL_P( valueitem));
				break;
			case IS_STRING:
				THROW_EXCEPTION( "number expected as scalar function variable value");
				break;
			case IS_DOUBLE:
				result.defineVariable( std::string( key, keylen), (double)Z_DVAL_P( valueitem));
				break;
			case IS_BOOL:
				result.defineVariable( std::string( key, keylen), (double)Z_BVAL_P( valueitem));
				break;
			default:
				THROW_EXCEPTION( "bad scalar function variable value");
				error = -1;
		}
	}
	catch (...)
	{
		THROW_EXCEPTION( "memory allocation error");
		error = -1;
	}
	return error;
}

int initFunctionVariableConfig( FunctionVariableConfig& result, zval* obj)
{
	int error = 0;
	switch (obj->type)
	{
		case IS_LONG:
			THROW_EXCEPTION( "unable to convert LONG to scalar function config");
			error = -1;
			break;
		case IS_STRING:
			THROW_EXCEPTION( "unable to convert STRING to scalar function config");
			error = -1;
			break;
		case IS_DOUBLE:
			THROW_EXCEPTION( "unable to convert DOUBLE to scalar function config");
			error = -1;
			break;
		case IS_BOOL:
			THROW_EXCEPTION( "unable to convert BOOL to scalar function config");
			error = -1;
			break;
		case IS_NULL:
			break;
		case IS_ARRAY:
		{
			zval **data;
			HashTable *hash;
			HashPosition ptr;
			hash = Z_ARRVAL_P(obj);
			for(
				zend_hash_internal_pointer_reset_ex(hash,&ptr);
				zend_hash_get_current_data_ex(hash,(void**)&data,&ptr) == SUCCESS;
				zend_hash_move_forward_ex(hash,&ptr))
			{
				char *name = 0;
				unsigned int name_len = 0;// length of string including 0 byte !
				unsigned long index;
				if (!zend_hash_get_current_key_ex( hash, &name, &name_len, &index, 0, &ptr) == HASH_KEY_IS_STRING)
				{
					THROW_EXCEPTION( "illegal key of sumarizer element (not a string)");
					error = -1;
					break;
				}
				if (0!=initFunctionVariable( result, name, name_len-1, *data))
				{
					error = -1;
					break;
				}
			}
			break;
		}
		case IS_OBJECT:
			THROW_EXCEPTION( "unable to convert OBJECT to scalar function config");
			error = -1;
			break;
		case IS_RESOURCE:
			THROW_EXCEPTION( "unable to convert RESOURCE to scalar function config");
			error = -1;
			break;
		default: 
			THROW_EXCEPTION( "unable to convert unknown type to scalar function config");
			error = -1;
			break;
	}
	return error;
}

static int defineQueryEvaluationFunctionParameter( WeightingConfig& result, const char* key, std::size_t keylen, zval* valueitem)
{
	int error = 0;
	try
	{
		if (keylen && key[0] == '.')
		{
			result.defineFeature( std::string( key+1, keylen-1), std::string( Z_STRVAL_P( valueitem)));
		}
		else
		{
			switch (Z_TYPE_P(valueitem))
			{
				case IS_LONG:
					result.defineParameter( std::string( key, keylen), Variant( (int)Z_LVAL_P( valueitem)));
					break;
				case IS_STRING:
					result.defineParameter( std::string( key, keylen), Variant( (char*)Z_STRVAL_P( valueitem)));
					break;
				case IS_DOUBLE:
					result.defineParameter( std::string( key, keylen), Variant( (double)Z_DVAL_P( valueitem)));
					break;
				case IS_BOOL:
					result.defineParameter( std::string( key, keylen), Variant( (int)Z_BVAL_P( valueitem)));
					break;
				default:
					THROW_EXCEPTION( "bad query evaluation function parameter value");
					error = -1;
			}
		}
	}
	catch (...)
	{
		THROW_EXCEPTION( "memory allocation error");
		error = -1;
	}
	return error;
}

static int defineSummarizationFunctionParameter( SummarizerConfig& result, const char* key, std::size_t keylen, zval* valueitem)
{
	int error = 0;
	try
	{
		if (keylen && key[0] == '.')
		{
			result.defineFeature( std::string( key+1, keylen-1), std::string( Z_STRVAL_P( valueitem)));
		}
		else if (keylen && key[0] == '$')
		{
			result.defineResultName( std::string( key+1, keylen-1), std::string( Z_STRVAL_P( valueitem)));
		}
		else
		{
			switch (Z_TYPE_P(valueitem))
			{
				case IS_LONG:
					result.defineParameter( std::string( key, keylen), Variant( (int)Z_LVAL_P( valueitem)));
					break;
				case IS_STRING:
					result.defineParameter( std::string( key, keylen), Variant( (char*)Z_STRVAL_P( valueitem)));
					break;
				case IS_DOUBLE:
					result.defineParameter( std::string( key, keylen), Variant( (double)Z_DVAL_P( valueitem)));
					break;
				case IS_BOOL:
					result.defineParameter( std::string( key, keylen), Variant( (int)Z_BVAL_P( valueitem)));
					break;
				default:
					THROW_EXCEPTION( "bad query evaluation function parameter value");
					error = -1;
			}
		}
	}
	catch (...)
	{
		THROW_EXCEPTION( "memory allocation error");
		error = -1;
	}
	return error;
}

template <class Object>
static int initQueryEvalFunctionConfig( Object& result, zval* obj, int (*DefineParameter)( Object& result, const char* key, std::size_t keylen, zval* valueitem))
{
	int error = 0;
	switch (obj->type)
	{
		case IS_LONG:
			THROW_EXCEPTION( "unable to convert LONG to function configuration");
			error = -1;
			break;
		case IS_STRING:
			THROW_EXCEPTION( "unable to convert STRING to function configuration");
			error = -1;
			break;
		case IS_DOUBLE:
			THROW_EXCEPTION( "unable to convert DOUBLE to function configuration");
			error = -1;
			break;
		case IS_BOOL:
			THROW_EXCEPTION( "unable to convert BOOL to function configuration");
			error = -1;
			break;
		case IS_NULL:
			break;
		case IS_ARRAY:
		{
			zval **data;
			HashTable *hash;
			HashPosition ptr;
			hash = Z_ARRVAL_P(obj);
			for(
				zend_hash_internal_pointer_reset_ex(hash,&ptr);
				zend_hash_get_current_data_ex(hash,(void**)&data,&ptr) == SUCCESS;
				zend_hash_move_forward_ex(hash,&ptr))
			{
				char *name = 0;
				unsigned int name_len = 0;// length of string including 0 byte !
				unsigned long index;
				if (!zend_hash_get_current_key_ex( hash, &name, &name_len, &index, 0, &ptr) == HASH_KEY_IS_STRING)
				{
					THROW_EXCEPTION( "illegal key of sumarizer element (not a string)");
					error = -1;
					break;
				}
				if (0!=DefineParameter( result, name, name_len-1, *data))
				{
					error = -1;
					break;
				}
			}
			break;
		}
		case IS_OBJECT:
			THROW_EXCEPTION( "unable to convert OBJECT to function configuration");
			error = -1;
			break;
		case IS_RESOURCE:
			THROW_EXCEPTION( "unable to convert RESOURCE to function configuration");
			error = -1;
			break;
		default: 
			THROW_EXCEPTION( "unable to convert unknown type to function configuration");
			error = -1;
			break;
	}
	return error;
}

int initSummarizerConfig( SummarizerConfig& result, zval* obj)
{
	return initQueryEvalFunctionConfig( result, obj, &defineSummarizationFunctionParameter);
}

int initWeightingConfig( WeightingConfig& result, zval* obj)
{
	return initQueryEvalFunctionConfig( result, obj, defineQueryEvaluationFunctionParameter);
}

template <class Expression>
static int initExpressionStructure( Expression& result, zval* obj)
{
	int error = 0;
	switch (obj->type)
	{
		case IS_LONG:
			THROW_EXCEPTION( "unable to convert LONG to query expression");
			error = -1;
			break;
		case IS_STRING:
			THROW_EXCEPTION( "unable to convert STRING to query expression");
			error = -1;
			break;
		case IS_DOUBLE:
			THROW_EXCEPTION( "unable to convert DOUBLE to query expression");
			error = -1;
			break;
		case IS_BOOL:
			THROW_EXCEPTION( "unable to convert BOOL to query expression");
			error = -1;
			break;
		case IS_NULL:
			break;
		case IS_ARRAY:
		{
			const char* variablename = 0;		//< variable assigned to this node
			const char* funcname = 0;		//< name of the function to call or type of term
			zval **data;
			HashTable *hash;
			HashPosition ptr;
			hash = Z_ARRVAL_P( obj);
			unsigned int arraysize = zend_hash_num_elements( hash);
			unsigned int aridx = 0;

			// [1] Initialize header:
			zend_hash_internal_pointer_reset_ex( hash, &ptr);
			while (aridx < 2 && zend_hash_get_current_data_ex( hash,(void**)&data,&ptr) == SUCCESS)
			{
				if ((Z_TYPE_PP(data) == IS_STRING))
				{
					const char* val = Z_STRVAL_P( *data);
					if (aridx == 0 && val && val[0] == '=')
					{
						if (val[1] == '\0')
						{
							THROW_EXCEPTION( "assignment variable name is empty");
							error = -1;
							break;
						}
						variablename = val+1;
						arraysize -= 1;
						zend_hash_move_forward_ex( hash,&ptr);
						++aridx;
					}
					else
					{
						funcname = val;
						zend_hash_move_forward_ex( hash,&ptr);
						++aridx;
						break;
					}
				}
				else
				{
					THROW_EXCEPTION( "string expected (function or type name) as first element of token or function definition");
					error = -1;
					break;
				}
			}
			if (error) break;
			if (!funcname)
			{
				THROW_EXCEPTION( "string expected (function or type name) as first element of token or function definition");
				error = -1;
				break;
			}
			// [2] Check if token definition, and push the token if it is:
			if (arraysize <= 2)
			{
				// Check, if we got a token definition
				const char* tokvalue = 0;
				if (zend_hash_get_current_data_ex( hash,(void**)&data,&ptr) != SUCCESS)
				{
					tokvalue = "";
				}
				else if ((Z_TYPE_PP(data) == IS_STRING))
				{
					tokvalue = Z_STRVAL_P( *data);
				}
				if (tokvalue)
				{
					result.pushTerm( funcname, tokvalue);
					if (variablename)
					{
						result.attachVariable( variablename);
					}
					return error;
				}
			}

			// [3] Get function cardinality and range:
			int range = 0;
			bool range_defined = false;
			unsigned int cardinality = 0;
			bool cardinality_defined = false;
			for(;
				zend_hash_get_current_data_ex( hash,(void**)&data,&ptr) == SUCCESS;
				zend_hash_move_forward_ex( hash,&ptr))
			{
				if ((Z_TYPE_PP(data) == IS_LONG))
				{
					if (!range_defined)
					{
						long val = Z_LVAL_PP( data);
						if (val >= std::numeric_limits<int>::max() || val <= std::numeric_limits<int>::min())
						{
							THROW_EXCEPTION( "range parameter exceeds allowed domain");
							error = -1;
							break;
						}
						range_defined = true;
						range = (int)val;
					}
					else if (!cardinality_defined)
					{
						long val = Z_LVAL_PP( data);
						if (val < 0 || val >= (long)std::numeric_limits<unsigned int>::max())
						{
							THROW_EXCEPTION( "cardinality parameter exceeds allowed domain");
							error = -1;
							break;
						}
						cardinality_defined = true;
						cardinality = (unsigned int)val;
					}
					else
					{
						break;
					}
				}
				else
				{
					break;
				}
			}
			if (error) break;

			// [4] Build arguments:
			unsigned int argcnt = 0;
			for(;
				zend_hash_get_current_data_ex( hash,(void**)&data,&ptr) == SUCCESS;
				zend_hash_move_forward_ex( hash,&ptr))
			{
				++argcnt;
				error = initExpressionStructure( result, *data);
				if (error) break;
			}
			if (error) break;

			// [5] Call the function and assign variable:
			result.pushExpression( funcname, argcnt, range, cardinality);
			if (variablename)
			{
				result.attachVariable( variablename);
			}
			break;
		}
		case IS_OBJECT:
			THROW_EXCEPTION( "unable to convert OBJECT to query expression");
			error = -1;
			break;
		case IS_RESOURCE:
			THROW_EXCEPTION( "unable to convert RESOURCE to query expression");
			error = -1;
			break;
		default: 
			THROW_EXCEPTION( "unable to convert unknown type to query expression");
			error = -1;
			break;
	}
	return error;
}

int initQueryExpression( QueryExpression& result, zval* obj)
{
	return initExpressionStructure( result, obj);
}

int initStringVector( std::vector<std::string>& result, zval* obj)
{
	int error = 0;
	switch (obj->type)
	{
		case IS_LONG:
			THROW_EXCEPTION( "unable to convert LONG to std::vector<std::string>");
			error = -1;
			break;
		case IS_STRING:
			try
			{
				result.push_back( Z_STRVAL_P( obj));
			}
			catch (...)
			{
				THROW_EXCEPTION( "memory allocation error");
				error = -1;
			}
			break;
		case IS_DOUBLE:
			THROW_EXCEPTION( "unable to convert DOUBLE to std::vector<std::string>");
			error = -1;
			break;
		case IS_BOOL:
			THROW_EXCEPTION( "unable to convert BOOL to std::vector<std::string>");
			error = -1;
			break;
		case IS_NULL:
			break;
		case IS_ARRAY:
		{
			zval **data;
			HashTable *hash;
			HashPosition ptr;
			hash = Z_ARRVAL_P(obj);
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
				try
				{
					result.push_back( std::string( Z_STRVAL_P(str)));
				}
				catch (...)
				{
					THROW_EXCEPTION( "memory allocation error");
					error = -1;
				}
				if (!is_str)
				{
					zval_dtor(&temp);
				}
			}
			break;
		}
		case IS_OBJECT:
			THROW_EXCEPTION( "unable to convert OBJECT to std::vector<std::string>");
			error = -1;
			break;
		case IS_RESOURCE:
			THROW_EXCEPTION( "unable to convert RESOURCE to std::vector<std::string>");
			error = -1;
			break;
		default: 
			THROW_EXCEPTION( "unable to convert unknown type to std::vector<std::string>");
			error = -1;
			break;
	}
	return error;
}

int initIntVector( std::vector<int>& result, zval* obj)
{
	int error = 0;
	switch (obj->type)
	{
		case IS_LONG:
			try
			{
				result.push_back( Z_LVAL_P( obj));
			}
			catch (...)
			{
				THROW_EXCEPTION( "memory allocation error");
				error = -1;
			}
			break;
		case IS_STRING:
			THROW_EXCEPTION( "unable to convert STRING to std::vector<int>");
			error = -1;
			break;
		case IS_DOUBLE:
			THROW_EXCEPTION( "unable to convert DOUBLE to std::vector<int>");
			error = -1;
			break;
		case IS_BOOL:
			THROW_EXCEPTION( "unable to convert BOOL to std::vector<int>");
			error = -1;
			break;
		case IS_NULL:
			break;
		case IS_ARRAY:
		{
			zval **data;
			HashTable *hash;
			HashPosition ptr;
			hash = Z_ARRVAL_P(obj);
			for(
				zend_hash_internal_pointer_reset_ex(hash,&ptr);
				zend_hash_get_current_data_ex(hash,(void**)&data,&ptr) == SUCCESS;
				zend_hash_move_forward_ex(hash,&ptr))
			{
				if (Z_TYPE_PP(data) != IS_LONG)
				{
					THROW_EXCEPTION( "expected array of integers");
					error = -1;
					break;
				}
				try
				{
					result.push_back( Z_LVAL_PP( data));
				}
				catch (...)
				{
					THROW_EXCEPTION( "memory allocation error");
					error = -1;
					break;
				}
			}
			break;
		}
		case IS_OBJECT:
			THROW_EXCEPTION( "unable to convert OBJECT to std::vector<int>");
			error = -1;
			break;
		case IS_RESOURCE:
			THROW_EXCEPTION( "unable to convert RESOURCE to std::vector<int>");
			error = -1;
			break;
		default: 
			THROW_EXCEPTION( "unable to convert unknown type to std::vector<int>");
			error = -1;
			break;
	}
	return error;
}

int initFloatVector( std::vector<double>& result, zval* obj)
{
	int error = 0;
	switch (obj->type)
	{
		case IS_LONG:
			try
			{
				result.push_back( Z_LVAL_P( obj));
			}
			catch (...)
			{
				THROW_EXCEPTION( "memory allocation error");
				error = -1;
			}
			break;
		case IS_STRING:
			THROW_EXCEPTION( "unable to convert STRING to std::vector<int>");
			error = -1;
			break;
		case IS_DOUBLE:
			try
			{
				result.push_back( Z_DVAL_P( obj));
			}
			catch (...)
			{
				THROW_EXCEPTION( "memory allocation error");
				error = -1;
			}
			break;
		case IS_BOOL:
			THROW_EXCEPTION( "unable to convert BOOL to std::vector<int>");
			error = -1;
			break;
		case IS_NULL:
			break;
		case IS_ARRAY:
		{
			zval **data;
			HashTable *hash;
			HashPosition ptr;
			hash = Z_ARRVAL_P(obj);
			for(
				zend_hash_internal_pointer_reset_ex(hash,&ptr);
				zend_hash_get_current_data_ex(hash,(void**)&data,&ptr) == SUCCESS;
				zend_hash_move_forward_ex(hash,&ptr))
			{
				if (Z_TYPE_PP(data) != IS_DOUBLE)
				{
					THROW_EXCEPTION( "expected array of double precision floats");
					error = -1;
					break;
				}
				try
				{
					result.push_back( Z_DVAL_PP( data));
				}
				catch (...)
				{
					THROW_EXCEPTION( "memory allocation error");
					error = -1;
					break;
				}
			}
			break;
		}
		case IS_OBJECT:
			THROW_EXCEPTION( "unable to convert OBJECT to std::vector<int>");
			error = -1;
			break;
		case IS_RESOURCE:
			THROW_EXCEPTION( "unable to convert RESOURCE to std::vector<int>");
			error = -1;
			break;
		default: 
			THROW_EXCEPTION( "unable to convert unknown type to std::vector<int>");
			error = -1;
			break;
	}
	return error;
}

class TermStatisticsBuilder
{
public:
	explicit TermStatisticsBuilder( TermStatistics* result_)
		:m_result(result_){}

	void set( const std::string& name, GlobalCounter value)
	{
		if (caseInsensitiveEquals( name, "df"))
		{
			m_result->set_df( value);
		}
		else
		{
			throw std::runtime_error( "unknown term statistics identifier (only 'df' known)");
		}
	}

private:
	TermStatistics* m_result;
};

class GlobalStatisticsBuilder
{
public:
	explicit GlobalStatisticsBuilder( GlobalStatistics* result_)
		:m_result(result_){}

	void set( const std::string& name, GlobalCounter value)
	{
		if (caseInsensitiveEquals( name, "nofdocs"))
		{
			m_result->set_nofdocs( value);
		}
		else
		{
			throw std::runtime_error( "unknown global statistics identifier (only 'nofdocs' known)");
		}
	}

private:
	GlobalStatistics* m_result;
};

template <class Builder, class Object>
static int initStructureObject( Object& result, zval* obj)
{
	Builder builder( &result);
	int error = 0;
	switch (obj->type)
	{
		case IS_LONG:
			THROW_EXCEPTION( "unable to convert LONG to structure");
			error = -1;
			break;
		case IS_STRING:
			THROW_EXCEPTION( "unable to convert STRING to structure");
			error = -1;
			break;
		case IS_DOUBLE:
			THROW_EXCEPTION( "unable to convert DOUBLE to structure");
			error = -1;
			break;
		case IS_BOOL:
			THROW_EXCEPTION( "unable to convert BOOL to structure");
			error = -1;
			break;
		case IS_NULL:
			break;
		case IS_ARRAY:
		{
			zval **data;
			HashTable *hash;
			HashPosition ptr;
			hash = Z_ARRVAL_P( obj);
			int argcnt = 0;
			for(
				zend_hash_internal_pointer_reset_ex(hash,&ptr);
				zend_hash_get_current_data_ex(hash,(void**)&data,&ptr) == SUCCESS;
				zend_hash_move_forward_ex(hash,&ptr),++argcnt)
			{
				zval val;
				INIT_ZVAL( val);
				char *key;
				unsigned int keysize;
				unsigned long index;

				if (!zend_hash_get_current_key_ex( hash, &key, &keysize, &index, 0, &ptr) == HASH_KEY_IS_STRING)
				{
					THROW_EXCEPTION( "key in structure is not a string (name of the item)");
					error = -1;
					break;
				}
				try
				{
					if (Z_TYPE_PP(data) != IS_LONG)
					{
						THROW_EXCEPTION( "integer value expected for element in statistics structure");
					}
					builder.set( std::string(key,keysize), Z_LVAL_PP( data));
				}
				catch (const std::runtime_error& err)
				{
					THROW_EXCEPTION( err.what());
					error = -1;
					break;
				}
				catch (const std::bad_alloc&)
				{
					THROW_EXCEPTION( "memory allocation error");
					error = -1;
					break;
				}
			}
			break;
		}
		case IS_OBJECT:
			THROW_EXCEPTION( "unable to convert OBJECT to function object");
			error = -1;
			break;
		case IS_RESOURCE:
			THROW_EXCEPTION( "unable to convert RESOURCE to function object");
			error = -1;
			break;
		default: 
			THROW_EXCEPTION( "unable to convert unknown type to function object");
			error = -1;
			break;
	}
	return error;
}

int initGlobalStatistics( GlobalStatistics& result, zval* obj)
{
	return initStructureObject<GlobalStatisticsBuilder,GlobalStatistics>( result, obj);
}

int initTermStatistics( TermStatistics& result, zval* obj)
{
	return initStructureObject<TermStatisticsBuilder,TermStatistics>( result, obj);
}

int getTermVector( zval*& result, const std::vector<Term>& ar)
{
	array_init_size( result, ar.size());
	std::vector<Term>::const_iterator ti = ar.begin(), te = ar.end();
	for (; ti != te; ++ti)
	{
		zval* term;
		MAKE_STD_ZVAL( term);
		object_init( term);
		add_property_string( term, "type", const_cast<char*>(ti->type().c_str()), 1);
		add_property_string( term, "value", const_cast<char*>(ti->value().c_str()), 1);
		add_property_long( term, "position", ti->position());
		add_next_index_zval( result, term);
	}
	return 0;
}

static int getSummaryElementVector( zval*& result, const std::vector<SummaryElement>& ar)
{
	array_init_size( result, ar.size());
	std::vector<SummaryElement>::const_iterator ai = ar.begin(), ae = ar.end();
	for (; ai != ae; ++ai)
	{
		zval* attr;
		MAKE_STD_ZVAL( attr);
		object_init( attr);
		add_property_string( attr, "name", const_cast<char*>(ai->name().c_str()), 1);
		add_property_string( attr, "value", const_cast<char*>(ai->value().c_str()), 1);
		add_property_double( attr, "weight", ai->weight());
		add_next_index_zval( result, attr);
	}
	return 0;
}

int getRankVector( zval*& result, const std::vector<Rank>& ar)
{
	array_init_size( result, ar.size());
	std::vector<Rank>::const_iterator ri = ar.begin(), re = ar.end();
	for (; ri != re; ++ri)
	{
		zval* rank;
		MAKE_STD_ZVAL( rank);
		object_init( rank);
		add_property_long( rank, "docno", ri->docno());
		add_property_double( rank, "weight", ri->weight());
		zval* rankattr;
		MAKE_STD_ZVAL( rankattr);
		getSummaryElementVector( rankattr, ri->summaryElements());
		add_property_zval( rank, "summaryElements", rankattr);

		add_next_index_zval( result, rank);
	}
	return 0;
}

int getVecRankVector( zval*& result, const std::vector<VecRank>& ar)
{
	array_init_size( result, ar.size());
	std::vector<VecRank>::const_iterator ri = ar.begin(), re = ar.end();
	for (; ri != re; ++ri)
	{
		zval* rank;
		MAKE_STD_ZVAL( rank);
		object_init( rank);
		add_property_long( rank, "index", ri->index());
		add_property_double( rank, "weight", ri->weight());

		add_next_index_zval( result, rank);
	}
	return 0;
}

int getStringVector( zval*& result, const std::vector<std::string>& ar)
{
	int error = 0;
	array_init_size( result, ar.size());
	std::vector<std::string>::const_iterator ri = ar.begin(), re = ar.end();
	for (; ri != re; ++ri)
	{
		error |= add_next_index_stringl( result, ri->c_str(), ri->size(), 1);
	}
	return error;
}

int getFloatVector( zval*& result, const std::vector<double>& ar)
{
	int error = 0;
	array_init_size( result, ar.size());
	std::vector<double>::const_iterator ri = ar.begin(), re = ar.end();
	for (; ri != re; ++ri)
	{
		error |= add_next_index_double( result, *ri);
	}
	return error;
}

int getIntVector( zval*& result, const std::vector<int>& ar)
{
	int error = 0;
	array_init_size( result, ar.size());
	std::vector<int>::const_iterator ri = ar.begin(), re = ar.end();
	for (; ri != re; ++ri)
	{
		error |= add_next_index_long( result, *ri);
	}
	return error;
}

int getQueryResult( zval*& result, const QueryResult& res)
{
	object_init( result);

	add_property_long( result, "evaluationPass", res.evaluationPass());
	add_property_long( result, "nofDocumentsRanked", res.nofDocumentsRanked());
	add_property_long( result, "nofDocumentsVisited", res.nofDocumentsVisited());

	zval* ranks;
	MAKE_STD_ZVAL( ranks);
	getRankVector( ranks, res.ranks());
	add_property_zval( result, "ranks", ranks);
	return 0;
}

int initPatternMatcher( PatternMatcher& matcher, zval* obj)
{
	int error = 0;
	switch (obj->type)
	{
		case IS_LONG:
			THROW_EXCEPTION( "unable to convert LONG to pattern matcher");
			error = -1;
			break;
		case IS_STRING:
			THROW_EXCEPTION( "unable to convert STRING to pattern matcher");
			error = -1;
			break;
		case IS_DOUBLE:
			THROW_EXCEPTION( "unable to convert DOUBLE to pattern matcher");
			error = -1;
			break;
		case IS_BOOL:
			THROW_EXCEPTION( "unable to convert BOOL to pattern matcher");
			error = -1;
			break;
		case IS_NULL:
			break;
		case IS_ARRAY:
		{
			zval **data;
			HashTable *hash;
			HashPosition ptr;
			hash = Z_ARRVAL_P( obj);
			int argcnt = 0;
			for(
				zend_hash_internal_pointer_reset_ex(hash,&ptr);
				zend_hash_get_current_data_ex(hash,(void**)&data,&ptr) == SUCCESS;
				zend_hash_move_forward_ex(hash,&ptr),++argcnt)
			{
				char *name = 0;
				unsigned int name_len = 0;// length of string including 0 byte !
				unsigned long index;
				if (!zend_hash_get_current_key_ex( hash, &name, &name_len, &index, 0, &ptr) == HASH_KEY_IS_STRING)
				{
					THROW_EXCEPTION( "illegal key of element (not a string)");
					error = -1;
					break;
				}
				if (0!=initExpressionStructure( matcher, *data))
				{
					error = -1;
					break;
				}
				try
				{
					if (name[0] == '-')
					{
						std::string ptname( name+1, name_len-1);
						matcher.definePattern( ptname.c_str(), false);
					}
					else
					{
						std::string ptname( name, name_len);
						matcher.definePattern( ptname.c_str(), true);
					}
				}
				catch (const std::bad_alloc&)
				{
					error = -1;
					break;
				}
			}
			break;
		}
		case IS_OBJECT:
			THROW_EXCEPTION( "unable to convert OBJECT to pattern matcher");
			error = -1;
			break;
		case IS_RESOURCE:
			THROW_EXCEPTION( "unable to convert RESOURCE to pattern matcher");
			error = -1;
			break;
		default:
			THROW_EXCEPTION( "unable to convert unknown type to pattern matcher");
			error = -1;
			break;
	}
	return error;
}



