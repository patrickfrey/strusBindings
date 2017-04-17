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
#include "strus/bindings/serialization.hpp"
#include "strus/numericVariant.hpp"
#include "strus/analyzer/document.hpp"
#include "strus/analyzer/query.hpp"
#include "strus/analyzer/term.hpp"
#include "strus/analyzer/attribute.hpp"
#include "strus/analyzer/metaData.hpp"
#include "strus/vectorStorageSearchInterface.hpp"
#include "strus/summaryElement.hpp"
#include "strus/resultDocument.hpp"
#include "strus/queryResult.hpp"
#include "statisticsMessage.hpp"
#include "queryAnalyzerStruct.hpp"
#include <string>
#include <cstring>

namespace strus {
namespace bindings {

class Serializer
{
public:
	static inline void serialize( Serialization& result, const double& val)
	{
		result.pushValue( ValueVariant( val));
	}
	static inline void serialize( Serialization& result, const ValueVariant::IntType& val)
	{
		result.pushValue( ValueVariant( val));
	}
	static inline void serialize_int( Serialization& result, const ValueVariant::IntType& val)
	{
		result.pushValue( ValueVariant( (ValueVariant::IntType)val));
	}
	static inline void serialize( Serialization& result, const ValueVariant::UIntType& val)
	{
		result.pushValue( ValueVariant( val));
	}
	static inline void serialize_uint( Serialization& result, const ValueVariant::UIntType& val)
	{
		result.pushValue( ValueVariant( (ValueVariant::UIntType)val));
	}
	static inline void serialize( Serialization& result, const bool& val)
	{
		result.pushValue( ValueVariant( (ValueVariant::IntType)val));
	}
	static inline void serialize_bool( Serialization& result, const bool& val)
	{
		result.pushValue( ValueVariant( (ValueVariant::IntType)val));
	}
	static inline void serialize( Serialization& result, const std::string& val)
	{
		result.pushValue( ValueVariant( val));
	}
	static inline void serialize( Serialization& result, const char* val)
	{
		result.pushValue( ValueVariant( val));
	}
	static inline void serialize( Serialization& result, const char* val, std::size_t valsize)
	{
		result.pushValue( ValueVariant( val, valsize));
	}
	static inline void serialize( Serialization& result, const NumericVariant& val)
	{
		result.pushValue( ValueVariant( val));
	}
	static void serialize( Serialization& result, const analyzer::Term& val);
	static void serialize( Serialization& result, const analyzer::Attribute& val);
	static void serialize( Serialization& result, const analyzer::MetaData& val);
	static void serialize( Serialization& result, const VectorStorageSearchInterface::Result& val);
	static void serialize( Serialization& result, const SummaryElement& val);
	static void serialize( Serialization& result, const analyzer::Document& val);
	static void serialize( Serialization& result, const analyzer::Query& val, const std::vector<QueryAnalyzerStruct::Operator>& operators, bool labeled);
	static void serialize( Serialization& result, const bindings::DocumentFrequencyChange& val);
	static void serialize( Serialization& result, const bindings::StatisticsMessage& val);
	static void serialize( Serialization& result, const ResultDocument& val);
	static void serialize( Serialization& result, const QueryResult& val);
	static void serialize( Serialization& result, const std::vector<VectorStorageSearchInterface::Result>& val);
	static void serialize( Serialization& result, const std::vector<analyzer::Term>& val);
	static void serialize( Serialization& result, const std::vector<std::string>& val);
	static void serialize( Serialization& result, const std::vector<int>& val);
	static void serialize( Serialization& result, const std::vector<double>& val);
	static void serialize( Serialization& result, const std::vector<analyzer::MetaData>& val);
	static void serialize( Serialization& result, const std::vector<analyzer::Attribute>& val);
	static void serialize( Serialization& result, const std::vector<StatisticsViewerInterface::DocumentFrequencyChange>& val);
	static void serialize( Serialization& result, const std::vector<strus::SummaryElement>& val);
	static void serialize( Serialization& result, const std::vector<ResultDocument>& val);

	template <typename TYPE>
	static void serializeStructMember( Serialization& result, const char* tagname, const TYPE& val)
	{
		result.pushName( tagname);
		serialize( result, val);
	}

private:
	template <typename TYPE>
	static void serializeArray( Serialization& result, const std::vector<TYPE>& val)
	{
		result.pushOpen();
		typename std::vector<TYPE>::const_iterator vi = val.begin(), ve = val.end();
		for (; vi != ve; ++vi)
		{
			serialize( result, *vi);
		}
		result.pushClose();
	}
	static void serializeIntArray( Serialization& result, const std::vector<int>& val)
	{
		result.pushOpen();
		typename std::vector<int>::const_iterator vi = val.begin(), ve = val.end();
		for (; vi != ve; ++vi)
		{
			serialize( result, (ValueVariant::IntType)*vi);
		}
		result.pushClose();
	}
};

}}//namespace
#endif

