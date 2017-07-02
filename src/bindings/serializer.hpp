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
#include <string>
#include <vector>
#include <utility>
#include <cstring>
#include <stdexcept>

namespace strus {
namespace bindings {

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
	static inline bool serialize_nothrow( papuga_Serialization* result, const papuga_UInt& val)
	{
		return papuga_Serialization_pushValue_uint( result, val);
	}
	static inline bool serialize_uint( papuga_Serialization* result, const papuga_UInt& val)
	{
		return papuga_Serialization_pushValue_uint( result, val);
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

	typedef StatisticsViewerInterface::DocumentFrequencyChange DocumentFrequencyChange;
	static bool serialize_nothrow( papuga_Serialization* result, const DocumentFrequencyChange& val);
	static bool serialize_nothrow( papuga_Serialization* result, const analyzer::QueryTerm& val, const char* variablename=0);
	static bool serialize_nothrow( papuga_Serialization* result, const analyzer::DocumentTerm& val);
	static bool serialize_nothrow( papuga_Serialization* result, const analyzer::DocumentAttribute& val);
	static bool serialize_nothrow( papuga_Serialization* result, const analyzer::DocumentMetaData& val);
	static bool serialize_nothrow( papuga_Serialization* result, const analyzer::Document& val);
	static bool serialize_nothrow( papuga_Serialization* result, const analyzer::DocumentClass& val);
	static bool serialize_nothrow( papuga_Serialization* result, const VectorStorageSearchInterface::Result& val);
	static bool serialize_nothrow( papuga_Serialization* result, const SummaryElement& val);
	static bool serialize_nothrow( papuga_Serialization* result, const TermExpression& val);
	static bool serialize_nothrow( papuga_Serialization* result, const MetaDataExpression& val, papuga_ErrorCode& err);
	static bool serialize_nothrow( papuga_Serialization* result, const ResultDocument& val);
	static bool serialize_nothrow( papuga_Serialization* result, const QueryResult& val);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<VectorStorageSearchInterface::Result>& val);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<std::string>& val);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<int>& val);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<double>& val);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<analyzer::DocumentTerm>& val);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<analyzer::DocumentMetaData>& val);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<analyzer::DocumentAttribute>& val);
	static bool serialize_nothrow( papuga_Serialization* result, StatisticsViewerInterface& val);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<strus::SummaryElement>& val);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<ResultDocument>& val);

	template <typename TYPE>
	static bool serializeStructMember( papuga_Serialization* result, const char* tagname, const TYPE& val)
	{
		bool rt = true;
		rt &= papuga_Serialization_pushName_charp( result, tagname);
		rt &= serialize_nothrow( result, val);
		return rt;
	}

	template <typename TYPE>
	static bool serializeArray( papuga_Serialization* result, const std::vector<TYPE>& val)
	{
		bool rt = true;
		rt &= papuga_Serialization_pushOpen( result);
		typename std::vector<TYPE>::const_iterator vi = val.begin(), ve = val.end();
		for (; vi != ve; ++vi)
		{
			rt &= serialize_nothrow( result, *vi);
		}
		rt &= papuga_Serialization_pushClose( result);
		return rt;
	}
	template <typename TYPE>
	static bool serializeIntArray( papuga_Serialization* result, const std::vector<TYPE>& val)
	{
		bool rt = true;
		rt &= papuga_Serialization_pushOpen( result);
		typename std::vector<int>::const_iterator vi = val.begin(), ve = val.end();
		for (; vi != ve; ++vi)
		{
			rt &= serialize_nothrow( result, (papuga_Int)*vi);
		}
		rt &= papuga_Serialization_pushClose( result);
		return rt;
	}
};

}}//namespace
#endif

