/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDINGS_SERIALIZER_HPP_INCLUDED
#define _STRUS_BINDINGS_SERIALIZER_HPP_INCLUDED
/// \brief Serializers of all data types needed for the language bindings
/// \file serializer.hpp
#include "papuga/serialization.h"
#include "papuga/errors.h"
#include "strus/numericVariant.hpp"
#include "strus/analyzer/document.hpp"
#include "strus/analyzer/queryTermExpression.hpp"
#include "strus/analyzer/documentClass.hpp"
#include "strus/vectorStorageSearchInterface.hpp"
#include "strus/summaryElement.hpp"
#include "strus/resultDocument.hpp"
#include "strus/queryResult.hpp"
#include "strus/statisticsViewerInterface.hpp"
#include "impl/value/termExpression.hpp"
#include "impl/value/metadataExpression.hpp"
#include "strus/bindingObjects.h"
#include <string>
#include <vector>
#include <utility>
#include <cstring>
#include <stdexcept>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>

namespace strus {
namespace bindings {

template <typename Struct>
class StructIdTemplate
{
public:
	static void structid(){}
};
template <>
class StructIdTemplate<analyzer::DocumentClass>
{
public:
	static int structid()		{return STRUS_BINDINGS_STRUCTID_DocumentClass;}
};


class Serializer
{
public:
	static void serialize( papuga_Serialization* result, const MetaDataExpression& param)
	{
		papuga_ErrorCode err;
		if (!serialize_nothrow( result, param, err)) throw std::runtime_error(papuga_ErrorCode_tostring(err));
	}
	static void serialize( papuga_Serialization* result, MetaDataExpression& param)
	{
		papuga_ErrorCode err;
		if (!serialize_nothrow( result, param, err)) throw std::runtime_error(papuga_ErrorCode_tostring(err));
	}
	template<typename SERVAL>
	static void serialize( papuga_Serialization* result, const SERVAL& param)
	{
		if (!serialize_nothrow( result, param)) throw std::bad_alloc();
	}
	template<typename SERVAL>
	static void serialize( papuga_Serialization* result, SERVAL& param)
	{
		if (!serialize_nothrow( result, param)) throw std::bad_alloc();
	}
	static inline bool serialize_nothrow( papuga_Serialization* result, const double& val)
	{
		return papuga_Serialization_pushValue_double( result, val);
	}
	static inline bool serialize_nothrow( papuga_Serialization* result, const papuga_Int& val)
	{
		return papuga_Serialization_pushValue_int( result, val);
	}
	static inline bool serialize_int( papuga_Serialization* result, const papuga_Int& val)
	{
		return papuga_Serialization_pushValue_int( result, val);
	}
	static inline bool serialize_nothrow( papuga_Serialization* result, const bool& val)
	{
		return papuga_Serialization_pushValue_int( result, val);
	}
	static inline bool serialize_bool( papuga_Serialization* result, const bool& val)
	{
		return papuga_Serialization_pushValue_int( result, val);
	}
	static inline bool serialize_nothrow( papuga_Serialization* result, const std::string& val)
	{
		return papuga_Serialization_pushValue_string( result, val.c_str(), val.size());
	}
	static inline bool serialize_nothrow( papuga_Serialization* result, const char* val)
	{
		return papuga_Serialization_pushValue_charp( result, val);
	}
	static inline bool serialize_nothrow( papuga_Serialization* result, const char* val, std::size_t valsize)
	{
		return papuga_Serialization_pushValue_string( result, val, valsize);
	}
	static inline bool serialize_nothrow( papuga_Serialization* result, const papuga_ValueVariant& val)
	{
		return papuga_Serialization_pushValue( result, &val);
	}
	static bool serialize_nothrow( papuga_Serialization* result, const NumericVariant& val);

	typedef std::vector<std::pair<std::string,std::string> > ConfigurationItemList;
	static bool serialize_nothrow( papuga_Serialization* result, const ConfigurationItemList& val);

	static bool serialize_nothrow( papuga_Serialization* result, const TermStatisticsChange& val, papuga_Allocator* allocator);
	static bool serialize_nothrow( papuga_Serialization* result, const analyzer::QueryTerm& val, const char* variablename=0);
	static bool serialize_nothrow( papuga_Serialization* result, const analyzer::DocumentTerm& val);
	static bool serialize_nothrow( papuga_Serialization* result, const analyzer::DocumentAttribute& val);
	static bool serialize_nothrow( papuga_Serialization* result, const analyzer::DocumentMetaData& val);
	static bool serialize_nothrow( papuga_Serialization* result, const analyzer::Document& val);
	static bool serialize_nothrow( papuga_Serialization* result, const analyzer::DocumentClass& val);
	static bool serialize_nothrow( papuga_Serialization* result, const VectorQueryResult& val);
	static bool serialize_nothrow( papuga_Serialization* result, const SummaryElement& val);
	static bool serialize_nothrow( papuga_Serialization* result, const TermExpression& val);
	static bool serialize_nothrow( papuga_Serialization* result, const MetaDataExpression& val, papuga_ErrorCode& err);
	static bool serialize_nothrow( papuga_Serialization* result, const ResultDocument& val);
	static bool serialize_nothrow( papuga_Serialization* result, const QueryResult& val);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<VectorQueryResult>& val);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<std::string>& val);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<int>& val);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<double>& val);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<analyzer::DocumentTerm>& val);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<analyzer::DocumentMetaData>& val);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<analyzer::DocumentAttribute>& val);
	static bool serialize_nothrow( papuga_Serialization* result, StatisticsViewerInterface& val, papuga_Allocator* allocator);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<strus::SummaryElement>& val);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<ResultDocument>& val);

	struct atomictype_ {};
	struct structtype_ {};
	struct maptype_ {};

	template <typename T>
	static typename boost::enable_if_c<
		boost::is_arithmetic<T>::value 
		|| boost::is_same<std::string,T>::value
		|| boost::is_same<char*,T>::value
		|| boost::is_same<const char*,T>::value
		|| boost::is_same<NumericVariant,T>::value
		|| boost::is_same<papuga_ValueVariant,T>::value
		,const atomictype_&>::type getCategory( const T&) { static atomictype_ rt; return rt;}

	template <typename T>
	static typename boost::enable_if_c<
		boost::is_same<analyzer::DocumentClass,T>::value
		,const structtype_&>::type getCategory( const T&) { static structtype_ rt; return rt;}

	template <typename T>
	static typename boost::enable_if_c<
		!(boost::is_arithmetic<T>::value 
		|| boost::is_same<std::string,T>::value
		|| boost::is_same<char*,T>::value
		|| boost::is_same<const char*,T>::value
		|| boost::is_same<NumericVariant,T>::value
		|| boost::is_same<papuga_ValueVariant,T>::value
		|| boost::is_same<analyzer::DocumentClass,T>::value)
		,const maptype_&>::type getCategory( const T&) { static maptype_ rt; return rt;}

	template<typename SERVAL>
	static bool serializeStructMemberValue( papuga_Serialization* result, const SERVAL& val, const atomictype_& category)
	{
		return serialize_nothrow( result, val);
	}
	template<typename SERVAL>
	static bool serializeStructMemberValue( papuga_Serialization* result, const SERVAL& val, const structtype_& category)
	{
		bool rt = true;
		rt &= papuga_Serialization_pushOpen_struct( result, StructIdTemplate<SERVAL>::structid());
		rt &= serialize_nothrow( result, val);
		rt &= papuga_Serialization_pushClose( result);
		return rt;
	}
	template<typename SERVAL>
	static bool serializeStructMemberValue( papuga_Serialization* result, const SERVAL& val, const maptype_& category)
	{
		bool rt = true;
		rt &= papuga_Serialization_pushOpen( result);
		rt &= serialize_nothrow( result, val);
		rt &= papuga_Serialization_pushClose( result);
		return rt;
	}

	template <typename TYPE>
	static bool serializeStructMember( papuga_Serialization* result, const char* tagname, const TYPE& val)
	{
		bool rt = true;
		rt &= papuga_Serialization_pushName_charp( result, tagname);
		rt &= serializeStructMemberValue( result, val, getCategory( val));
		return rt;
	}

	template <typename TYPE>
	static bool serializeArray( papuga_Serialization* result, const std::vector<TYPE>& val)
	{
		bool rt = true;
		typename std::vector<TYPE>::const_iterator vi = val.begin(), ve = val.end();
		for (; vi != ve; ++vi)
		{
			rt &= serializeStructMemberValue( result, *vi, getCategory( *vi));
		}
		return rt;
	}
	template <typename TYPE>
	static bool serializeIntArray( papuga_Serialization* result, const std::vector<TYPE>& val)
	{
		bool rt = true;
		typename std::vector<int>::const_iterator vi = val.begin(), ve = val.end();
		for (; vi != ve; ++vi)
		{
			rt &= serialize_nothrow( result, (papuga_Int)*vi);
		}
		return rt;
	}
};

}}//namespace
#endif

