/*
---------------------------------------------------------------------
    The C++ library strus implements basic operations to build
    a search engine for structured search on unstructured data.

    Copyright (C) 2013,2014 Patrick Frey

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

--------------------------------------------------------------------

	The latest version of strus can be found at 'http://github.com/patrickfrey/strus'
	For documentation see 'http://patrickfrey.github.com/strus'

--------------------------------------------------------------------
*/
#include <zend.h>
#include <zend_API.h>
#include <zend_exceptions.h>
#include "objInitializers.hpp"

#define THROW_EXCEPTION( MSG) zend_throw_exception( NULL, const_cast<char*>( MSG), 0 TSRMLS_CC)

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

template <class Object>
static int defineQueryEvaluationFunctionParameter( Object& result, const char* key, std::size_t keylen, zval* valueitem)
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

template <class Object>
static int initQueryEvalFunctionConfig( Object& result, zval* obj)
{
	int error = 0;
	switch (obj->type)
	{
		case IS_LONG:
			THROW_EXCEPTION( "unable to convert LONG to SummarizerConfig");
			error = -1;
			break;
		case IS_STRING:
			THROW_EXCEPTION( "unable to convert STRING to SummarizerConfig");
			error = -1;
			break;
		case IS_DOUBLE:
			THROW_EXCEPTION( "unable to convert DOUBLE to SummarizerConfig");
			error = -1;
			break;
		case IS_BOOL:
			THROW_EXCEPTION( "unable to convert BOOL to SummarizerConfig");
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
				if (0!=defineQueryEvaluationFunctionParameter( result, name, name_len-1, *data))
				{
					error = -1;
					break;
				}
			}
			break;
		}
		case IS_OBJECT:
			THROW_EXCEPTION( "unable to convert OBJECT to SummarizerConfig");
			error = -1;
			break;
		case IS_RESOURCE:
			THROW_EXCEPTION( "unable to convert RESOURCE to SummarizerConfig");
			error = -1;
			break;
		default: 
			THROW_EXCEPTION( "unable to convert unknown type to SummarizerConfig");
			error = -1;
			break;
	}
	return error;
}

int initSummarizerConfig( SummarizerConfig& result, zval* obj)
{
	return initQueryEvalFunctionConfig( result, obj);
}

int initWeightingConfig( WeightingConfig& result, zval* obj)
{
	return initQueryEvalFunctionConfig( result, obj);
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

int getTermVector( zval* result, const std::vector<Term>& ar)
{
	array_init( result);
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

int getRankVector( zval* result, const std::vector<Rank>& ar)
{
	array_init( result);
	std::vector<Rank>::const_iterator ri = ar.begin(), re = ar.end();
	for (; ri != re; ++ri)
	{
		zval* rank;
		MAKE_STD_ZVAL( rank);
		object_init( rank);
		add_property_long( rank, "docno", ri->docno());
		add_property_double( rank, "weight", ri->weight());

		std::vector<RankAttribute>::const_iterator ai = ri->attributes().begin(), ae = ri->attributes().end();
		std::multimap<std::string,const char*> attr;
		try
		{
			for (; ai != ae; ++ai)
			{
				attr.insert( std::pair<std::string,const char*>( ai->name(), ai->value().c_str()));
			}
		}
		catch (...)
		{
			THROW_EXCEPTION( "memory allocation error");
			return 0;
		}
		std::multimap<std::string,const char*>::const_iterator
			mi = attr.begin(), me = attr.end();
		for (; mi != me; ++mi)
		{
			std::multimap<std::string,const char*>::const_iterator next_mi = mi;
			++next_mi;
			if (next_mi == me || next_mi->first != mi->first)
			{
				// ... single attribute
				add_property_string( rank, const_cast<char*>(mi->first.c_str()), mi->second, 1);
			}
			else
			{
				// ... multi element attribute
				zval* multiattr;
				MAKE_STD_ZVAL( multiattr);
				array_init( multiattr);
				while (next_mi != me && next_mi->first == mi->first)
				{
					add_next_index_string( multiattr, mi->second, 1);
					++next_mi;
					++mi;
				}
				add_next_index_string( multiattr, mi->second, 1);
				add_property_zval( rank, mi->first.c_str(), multiattr);
			}
		}
		add_next_index_zval( result, rank);
	}
	return 0;
}



