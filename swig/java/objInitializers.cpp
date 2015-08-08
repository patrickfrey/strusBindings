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

static int ERROR( JNIEnv* jenv, const char* msg)
{
	jclass clazz = jenv->FindClass("java/lang/Exception");
	jenv->ThrowNew( clazz, msg);
	return -1;
}

int initVariant( Variant& result, JNIEnv* jenv, jobject obj)
{
	jclass variant_class = jenv->FindClass("Variant");
	jclass objCls = jenv->GetObjectClass( obj);
	jmethodID mGetInt = jenv->GetMethodID( objCls, "intValue","()I;");
	if(mGetInt == NULL)
	{
		return ERROR( jenv, "cannot convert parameter to int");
	}
	result.assignInt( jenv->CallIntMethod( obj, mGetInt));
}

int initTokenizer( Tokenizer& result, JNIEnv* jenv, jobject obj)
{
	
}

int initNormalizer( Normalizer& result, JNIEnv* jenv, jobject obj)
{
	
}

int initNormalizerList( std::vector<Normalizer>& result, JNIEnv* jenv, jobject obj)
{
	
}

int initAggregator( Aggregator& result, JNIEnv* jenv, jobject obj)
{
	
}

int initSummarizerConfig( SummarizerConfig& result, JNIEnv* jenv, jobject obj)
{
	
}

int initWeightingConfig( WeightingConfig& result, JNIEnv* jenv, jobject obj)
{
	
}

int initStringVector( std::vector<std::string>& result, JNIEnv* jenv, jobject obj)
{
	
}

jobject getTermVector( JNIEnv* jenv, const std::vector<Term>& ar)
{
	
}

jobject getRankVector( JNIEnv* jenv, const std::vector<Rank>& ar)
{
	
}


