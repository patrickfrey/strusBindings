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
#include "strus/bindings/statisticsMessage.hpp"
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
	static inline void serialize( Serialization& result, const ValueVariant::UIntType& val)
	{
		result.pushValue( ValueVariant( val));
	}
	static inline void serialize( Serialization& result, const bool& val)
	{
		result.pushValue( ValueVariant( val));
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
	static inline void serialize( Serialization& result, const analyzer::Query::Instruction::OpCode& val)
	{
		result.pushValue( ValueVariant( analyzer::Query::Instruction::opCodeName( val)));
	}
	static inline void serialize( Serialization& result, const analyzer::Query::Element::Type& val)
	{
		result.pushValue( ValueVariant( analyzer::Query::Element::typeName( val)));
	}
	static void serialize( Serialization& result, const analyzer::Term& val)
	{
		serializeStructMember( result, "type", val.type());
		serializeStructMember( result, "value", val.value());
		serializeStructMember( result, "pos", (ValueVariant::UIntType)val.pos());
		serializeStructMember( result, "len", (ValueVariant::UIntType)val.len());
	}
	static void serialize( Serialization& result, const analyzer::Attribute& val)
	{
		serializeStructMember( result, "name", val.name());
		serializeStructMember( result, "value", val.value());
	}
	static void serialize( Serialization& result, const analyzer::MetaData& val)
	{
		serializeStructMember( result, "name", val.name());
		serializeStructMember( result, "value", val.value());
	}
	static void serialize( Serialization& result, const analyzer::Query::Element& val)
	{
		serializeStructMember( result, "type", val.type());
		serializeStructMember( result, "idx", val.idx());
		serializeStructMember( result, "pos", (ValueVariant::UIntType)val.pos());
		serializeStructMember( result, "len", (ValueVariant::UIntType)val.len());
		serializeStructMember( result, "fieldno", (ValueVariant::UIntType)val.fieldNo());
	}
	static void serialize( Serialization& result, const analyzer::Query::Element& val)
	{
		serializeStructMember( result, "type", val.type());
		serializeStructMember( result, "idx", val.idx());
		serializeStructMember( result, "pos", (ValueVariant::UIntType)val.pos());
		serializeStructMember( result, "len", (ValueVariant::UIntType)val.len());
		serializeStructMember( result, "field", (ValueVariant::UIntType)val.field());
	}
	static void serialize( Serialization& result, const analyzer::Query::Instruction& val)
	{
		serializeStructMember( result, "type", val.type());
		serializeStructMember( result, "idx", val.idx());
		serializeStructMember( result, "nofOperands", (ValueVariant::UIntType)val.nofOperands());
	}
	static void serialize( Serialization& result, const VectorStorageSearchInterface::Result& val)
	{
		serializeStructMember( result, "featidx", (ValueVariant::IntType)val.featidx());
		serializeStructMember( result, "weight", val.weight());
	}
	static void serialize( Serialization& result, const SummaryElement& val)
	{
		serializeStructMember( result, "name", val.name());
		serializeStructMember( result, "value", val.value());
		serializeStructMember( result, "weight", val.weight());
		serializeStructMember( result, "index", (ValueVariant::IntType)val.index());
	}
	static void serialize( Serialization& result, const analyzer::Document& val)
	{
		serializeStructMember( result, "subDocumentTypeName", val.subDocumentTypeName());
		serializeStructMemberArray( result, "metadata", val.metadata());
		serializeStructMemberArray( result, "attributes", val.attributes());
		serializeStructMemberArray( result, "searchTerms", val.searchTerms());
		serializeStructMemberArray( result, "forwardTerms", val.forwardTerms());
	}
	static void serialize( Serialization& result, const analyzer::Query& val)
	{
		serializeStructMemberArray( result, "metadata", val.metadata());
		serializeStructMemberArray( result, "searchTerms", val.searchTerms());
		serializeStructMemberArray( result, "elements", val.elements());
		serializeStructMemberArray( result, "instructions", val.instructions());
	}
	static void serialize( Serialization& result, const bindings::DocumentFrequencyChange& val)
	{
		serializeStructMember( result, "type", val.type());
		serializeStructMember( result, "value", val.value());
		serializeStructMember( result, "increment", (ValueVariant::IntType)val.increment());
	}
	static void serialize( Serialization& result, const bindings::StatisticsMessage& val)
	{
		serializeStructMemberArray( result, "dflist", val.dflist());
		serializeStructMember( result, "nofdocs", (ValueVariant::IntType)val.nofdocs());
	}
	static void serialize( Serialization& result, const ResultDocument& val)
	{
		serializeStructMember( result, "docno", (ValueVariant::IntType)val.docno());
		serializeStructMember( result, "weight", val.weight());
		serializeStructMemberArray( result, "summaryElements", val.summaryElements());
	}
	static void serialize( Serialization& result, const QueryResult& val)
	{
		serializeStructMember( result, "evaluationPass", (ValueVariant::UIntType)val.evaluationPass());
		serializeStructMember( result, "nofRanked", (ValueVariant::UIntType)val.nofRanked());
		serializeStructMember( result, "nofVisited", (ValueVariant::UIntType)val.nofVisited());
		serializeStructMemberArray( result, "ranks", val.ranks());
	}
	static void serialize( Serialization& result, const std::vector<VectorStorageSearchInterface::Result>& val)
	{
		serializeArray( result, val);
	}
	static void serialize( Serialization& result, const std::vector<analyzer::Term>& val)
	{
		serializeArray( result, val);
	}
	static void serialize( Serialization& result, const std::vector<std::string>& val)
	{
		serializeArray( result, val);
	}
	static void serialize( Serialization& result, const std::vector<int>& val)
	{
		serializeArray<ValueVariant::IntType>( result, val);
	}
	static void serialize( Serialization& result, const std::vector<double>& val)
	{
		serializeArray( result, val);
	}

private:
	template <typename TYPE>
	static void serializeStructMember( Serialization& result, const char* tagname, const TYPE& val)
	{
		result.pushOpen( tagname);
		serialize( result, val);
		result.pushClose();
	}
	template <typename TYPE>
	static void serializeArray( Serialization& result, const std::vector<TYPE>& val)
	{
		std::vector<TYPE>::const_iterator vi = val.begin(), ve = val.end();
		for (; vi != ve; ++vi)
		{
			result.pushIndex();
			serialize( result, *vi);
		}
	}
	template <typename TYPE>
	static void serializeStructMemberArray( Serialization& result, const char* tagname, const std::vector<TYPE>& val)
	{
		result.pushOpen( tagname);
		serializeArray( result, val);
		result.pushClose();
	}
};

}}//namespace
#endif

