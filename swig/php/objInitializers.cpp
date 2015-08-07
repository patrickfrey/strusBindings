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

int initVariant( Variant& result, zval* obj)
{
	try
	{
	}
	catch (...)
	{
		zend_throw_exception( NULL, const_cast<char*>("memory allocation error in variant type initialization"), 0 TSRMLS_CC);
		return -1;
	}
	return 0;
}

template <class Object>
static int initFunctionObject( Object& result, zval* obj)
{
	int error = 0;
	try
	{
	}
	catch (...)
	{
		zend_error( E_ERROR, "memory allocation error in variant type initialization");
		return -1;
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
	try
	{
	}
	catch ( const std::exception& err)
	{
		zend_error( E_ERROR, err.what());
		return -1;
	}
	return error;
}

int initAggregator( Aggregator& result, zval* obj)
{
	return initFunctionObject( result, obj);
}

template <class Object>
static int defineQueryEvaluationFunctionParameter( Object& result, zval* keyitem, zval* valueitem)
{
	int error = 0;
	try
	{
	}
	catch ( const std::exception& err)
	{
		zend_error( E_ERROR, err.what());
		return -1;
	}
	return error;
}

template <class Object>
static int initQueryEvalFunctionConfig( Object& result, zval* obj)
{
	int error = 0;
	try
	{
	}
	catch ( const std::exception& err)
	{
		zend_error( E_ERROR, err.what());
		return -1;
	}
	return error;
}

int initSummarizerConfig( SummarizerConfig& result, zval* obj)
{
	return initQueryEvalFunctionConfig( result, obj);
}

int initWeightingConfig( SummarizerConfig& result, zval* obj)
{
	return initQueryEvalFunctionConfig( result, obj);
}

int initStringVector( std::vector<std::string>& result, zval* obj)
{
	int error = 0;
	try
	{
	}
	catch ( const std::exception& err)
	{
		zend_error( E_ERROR, err.what());
		return -1;
	}
	return error;
}

zval* getTermVector( const std::vector<Term>& ar)
{
	zval* rt = 0;
	try
	{
	}
	catch ( const std::exception& err)
	{
		zend_error( E_ERROR, err.what());
		return 0;
	}
	return rt;
}

static zval* getRankAttributeVector( const std::vector<RankAttribute>& ar)
{
	zval* rt = 0;
	try
	{
	}
	catch ( const std::exception& err)
	{
		zend_error( E_ERROR, err.what());
		return 0;
	}
	return rt;
}

zval* getRankVector( const std::vector<Rank>& ar)
{
	zval* rt = 0;
	try
	{
	}
	catch ( const std::exception& err)
	{
		zend_error( E_ERROR, err.what());
		return 0;
	}
	return rt;
}



