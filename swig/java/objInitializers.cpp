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
#include "objInitializers.hpp"
#include "strus/private/dll_tags.hpp"

static int ERROR( JNIEnv* jenv, const char* msg)
{
	jclass clazz = jenv->FindClass("java/lang/Exception");
	jenv->ThrowNew( clazz, msg);
	return -1;
}

template <typename ObjType>
static int initVector( std::vector<ObjType>& result, JNIEnv* jenv, jlong objidx)
{
	try
	{
		typedef std::vector<ObjType>* ObjPtrType;
		std::vector<ObjType>* objptr = *(ObjPtrType*)(&objidx);
		typename std::vector<ObjType>::const_iterator oi = objptr->begin(), oe = objptr->end();
		for (; oi != oe; ++oi)
		{
			result.push_back( *oi);
		}
	}
	catch (const std::bad_alloc& err)
	{
		return ERROR( jenv, "memory allocation error");
	}
	catch (const std::runtime_error& err)
	{
		return ERROR( jenv, err.what());
	}
	return 0;
}

// int initVariant( Variant& result, JNIEnv* jenv, jlong objptr){}
// int initTokenizer( Tokenizer& result, JNIEnv* jenv, jlong objptr){}
// int initNormalizer( Normalizer& result, JNIEnv* jenv, jlong objptr){}

DLL_PUBLIC int strus::initNormalizerVector( std::vector<Normalizer>& result, JNIEnv* jenv, jlong objidx)
{
	return initVector<Normalizer>( result, jenv, objidx);
}

// int initAggregator( Aggregator& result, JNIEnv* jenv, jlong objptr){}
// int initSummarizerConfig( SummarizerConfig& result, JNIEnv* jenv, jlong objptr){}
// int initWeightingConfig( WeightingConfig& result, JNIEnv* jenv, jlong objptr){}

DLL_PUBLIC int strus::initStringVector( std::vector<std::string>& result, JNIEnv* jenv, jlong objidx)
{
	return initVector<std::string>( result, jenv, objidx);
}


