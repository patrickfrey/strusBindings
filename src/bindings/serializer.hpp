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
#include "strus/numericVariant.hpp"
#include "strus/analyzer/document.hpp"
#include "strus/analyzer/query.hpp"
#include "strus/analyzer/term.hpp"
#include "strus/analyzer/attribute.hpp"
#include "strus/analyzer/metaData.hpp"
#include "strus/analyzer/documentClass.hpp"
#include "strus/vectorStorageSearchInterface.hpp"
#include "strus/summaryElement.hpp"
#include "strus/resultDocument.hpp"
#include "strus/queryResult.hpp"
#include "strus/statisticsViewerInterface.hpp"
#include "impl/analyzedQuery.hpp"
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
	template<typename SERVAL>
	static void serialize( papuga_Serialization* result, const SERVAL& param)
	{
		if (!serialize_nothrow( result, param)) throw std::bad_alloc();
	}

	static inline bool serialize_nothrow( papuga_Serialization* result, const double& val)
	{
		return papuga_Serialization_pushValue_double( result, val);
	}
	static inline bool serialize_nothrow( papuga_Serialization* result, const papuga_IntType& val)
	{
		return papuga_Serialization_pushValue_int( result, val);
	}
	static inline bool serialize_int( papuga_Serialization* result, const papuga_IntType& val)
	{
		return papuga_Serialization_pushValue_int( result, val);
	}
	static inline bool serialize_nothrow( papuga_Serialization* result, const papuga_UIntType& val)
	{
		return papuga_Serialization_pushValue_uint( result, val);
	}
	static inline bool serialize_uint( papuga_Serialization* result, const papuga_UIntType& val)
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
	static bool serialize_nothrow( papuga_Serialization* result, const NumericVariant& val);

	typedef std::vector<std::pair<std::string,std::string> > ConfigurationItemList;
	static bool serialize_nothrow( papuga_Serialization* result, const ConfigurationItemList& val);

	typedef StatisticsViewerInterface::DocumentFrequencyChange DocumentFrequencyChange;
	static bool serialize_nothrow( papuga_Serialization* result, const DocumentFrequencyChange& val);
	static bool serialize_nothrow( papuga_Serialization* result, const analyzer::Term& val);
	static bool serialize_nothrow( papuga_Serialization* result, const analyzer::Attribute& val);
	static bool serialize_nothrow( papuga_Serialization* result, const analyzer::MetaData& val);
	static bool serialize_nothrow( papuga_Serialization* result, const analyzer::DocumentClass& val);
	static bool serialize_nothrow( papuga_Serialization* result, const VectorStorageSearchInterface::Result& val);
	static bool serialize_nothrow( papuga_Serialization* result, const SummaryElement& val);
	static bool serialize_nothrow( papuga_Serialization* result, const analyzer::Document& val);
	static bool serialize_nothrow( papuga_Serialization* result, const AnalyzedQuery& query);
	static bool serialize_nothrow( papuga_Serialization* result, const ResultDocument& val);
	static bool serialize_nothrow( papuga_Serialization* result, const QueryResult& val);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<VectorStorageSearchInterface::Result>& val);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<analyzer::Term>& val);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<std::string>& val);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<int>& val);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<double>& val);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<analyzer::MetaData>& val);
	static bool serialize_nothrow( papuga_Serialization* result, const std::vector<analyzer::Attribute>& val);
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

private:
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
			rt &= serialize_nothrow( result, (papuga_IntType)*vi);
		}
		rt &= papuga_Serialization_pushClose( result);
		return rt;
	}
};

}}//namespace
#endif

