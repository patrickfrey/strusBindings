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
#include "papuga/serialization.hpp"
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
#include "queryAnalyzerStruct.hpp"
#include <string>
#include <vector>
#include <utility>
#include <cstring>

namespace strus {
namespace bindings {

class Serializer
{
public:
	static inline void serialize( papuga::Serialization& result, const double& val)
	{
		result.pushValue( papuga::ValueVariant( val));
	}
	static inline void serialize( papuga::Serialization& result, const papuga::ValueVariant::IntType& val)
	{
		result.pushValue( papuga::ValueVariant( val));
	}
	static inline void serialize_int( papuga::Serialization& result, const papuga::ValueVariant::IntType& val)
	{
		result.pushValue( papuga::ValueVariant( (papuga::ValueVariant::IntType)val));
	}
	static inline void serialize( papuga::Serialization& result, const papuga::ValueVariant::UIntType& val)
	{
		result.pushValue( papuga::ValueVariant( val));
	}
	static inline void serialize_uint( papuga::Serialization& result, const papuga::ValueVariant::UIntType& val)
	{
		result.pushValue( papuga::ValueVariant( (papuga::ValueVariant::UIntType)val));
	}
	static inline void serialize( papuga::Serialization& result, const bool& val)
	{
		result.pushValue( papuga::ValueVariant( (papuga::ValueVariant::IntType)val));
	}
	static inline void serialize_bool( papuga::Serialization& result, const bool& val)
	{
		result.pushValue( papuga::ValueVariant( (papuga::ValueVariant::IntType)val));
	}
	static inline void serialize( papuga::Serialization& result, const std::string& val)
	{
		result.pushValue( papuga::ValueVariant( val));
	}
	static inline void serialize( papuga::Serialization& result, const char* val)
	{
		result.pushValue( papuga::ValueVariant( val));
	}
	static inline void serialize( papuga::Serialization& result, const char* val, std::size_t valsize)
	{
		result.pushValue( papuga::ValueVariant( val, valsize));
	}
	static void serialize( papuga::Serialization& result, const NumericVariant& val);

	typedef std::vector<std::pair<std::string,std::string> > ConfigurationItemList;
	static void serialize( papuga::Serialization& result, const ConfigurationItemList& val);

	typedef StatisticsViewerInterface::DocumentFrequencyChange DocumentFrequencyChange;
	static void serialize( papuga::Serialization& result, const DocumentFrequencyChange& val);
	static void serialize( papuga::Serialization& result, const analyzer::Term& val);
	static void serialize( papuga::Serialization& result, const analyzer::Attribute& val);
	static void serialize( papuga::Serialization& result, const analyzer::MetaData& val);
	static void serialize( papuga::Serialization& result, const analyzer::DocumentClass& val);
	static void serialize( papuga::Serialization& result, const VectorStorageSearchInterface::Result& val);
	static void serialize( papuga::Serialization& result, const SummaryElement& val);
	static void serialize( papuga::Serialization& result, const analyzer::Document& val);
	static void serialize( papuga::Serialization& result, const analyzer::Query& val, const std::vector<QueryAnalyzerStruct::Operator>& operators, bool labeled);
	static void serialize( papuga::Serialization& result, const ResultDocument& val);
	static void serialize( papuga::Serialization& result, const QueryResult& val);
	static void serialize( papuga::Serialization& result, const std::vector<VectorStorageSearchInterface::Result>& val);
	static void serialize( papuga::Serialization& result, const std::vector<analyzer::Term>& val);
	static void serialize( papuga::Serialization& result, const std::vector<std::string>& val);
	static void serialize( papuga::Serialization& result, const std::vector<int>& val);
	static void serialize( papuga::Serialization& result, const std::vector<double>& val);
	static void serialize( papuga::Serialization& result, const std::vector<analyzer::MetaData>& val);
	static void serialize( papuga::Serialization& result, const std::vector<analyzer::Attribute>& val);
	static void serialize( papuga::Serialization& result, const std::vector<StatisticsViewerInterface::DocumentFrequencyChange>& val);
	static void serialize( papuga::Serialization& result, const std::vector<strus::SummaryElement>& val);
	static void serialize( papuga::Serialization& result, const std::vector<ResultDocument>& val);

	template <typename TYPE>
	static void serializeStructMember( papuga::Serialization& result, const char* tagname, const TYPE& val)
	{
		result.pushName( tagname);
		serialize( result, val);
	}

private:
	template <typename TYPE>
	static void serializeArray( papuga::Serialization& result, const std::vector<TYPE>& val)
	{
		result.pushOpen();
		typename std::vector<TYPE>::const_iterator vi = val.begin(), ve = val.end();
		for (; vi != ve; ++vi)
		{
			serialize( result, *vi);
		}
		result.pushClose();
	}
	static void serializeIntArray( papuga::Serialization& result, const std::vector<int>& val)
	{
		result.pushOpen();
		typename std::vector<int>::const_iterator vi = val.begin(), ve = val.end();
		for (; vi != ve; ++vi)
		{
			serialize( result, (papuga::ValueVariant::IntType)*vi);
		}
		result.pushClose();
	}
};

}}//namespace
#endif

