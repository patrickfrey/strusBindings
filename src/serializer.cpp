/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Serializers of all data types needed for the language bindings
/// \file serializer.hpp
#include "serializer.hpp"
#include "internationalization.hpp"

using namespace strus;
using namespace strus::bindings;

static void serialize_positional( Serialization& result, const analyzer::Term& val)
{
	Serializer::serialize( result, val.type());
	Serializer::serialize( result, val.value());
	Serializer::serialize( result, (ValueVariant::UIntType)val.pos());
	Serializer::serialize( result, (ValueVariant::UIntType)val.len());
}
void Serializer::serialize( Serialization& result, const analyzer::Term& val)
{
	serializeStructMember( result, "type", val.type());
	serializeStructMember( result, "value", val.value());
	serializeStructMember( result, "pos", (ValueVariant::UIntType)val.pos());
	serializeStructMember( result, "len", (ValueVariant::UIntType)val.len());
}
void Serializer::serialize( Serialization& result, const analyzer::Attribute& val)
{
	serializeStructMember( result, "name", val.name());
	serializeStructMember( result, "value", val.value());
}
static void serialize_positional( Serialization& result, const analyzer::MetaData& val)
{
	Serializer::serialize( result, val.name());
	Serializer::serialize( result, val.value());
}
void Serializer::serialize( Serialization& result, const analyzer::MetaData& val)
{
	serializeStructMember( result, "name", val.name());
	serializeStructMember( result, "value", val.value());
}
void Serializer::serialize( Serialization& result, const VectorStorageSearchInterface::Result& val)
{
	serializeStructMember( result, "featidx", (ValueVariant::IntType)val.featidx());
	serializeStructMember( result, "weight", val.weight());
}
void Serializer::serialize( Serialization& result, const SummaryElement& val)
{
	serializeStructMember( result, "name", val.name());
	serializeStructMember( result, "value", val.value());
	serializeStructMember( result, "weight", val.weight());
	serializeStructMember( result, "index", (ValueVariant::IntType)val.index());
}
void Serializer::serialize( Serialization& result, const analyzer::Document& val)
{
	serializeStructMember( result, "doctype", val.subDocumentTypeName());
	serializeStructMember( result, "metadata", val.metadata());
	serializeStructMember( result, "attributes", val.attributes());
	serializeStructMember( result, "searchindex", val.searchIndexTerms());
	serializeStructMember( result, "forwardindex", val.forwardIndexTerms());
}
static void serialize_labeled( Serialization& result, const analyzer::Query& val, const std::vector<QueryAnalyzerStruct::Operator>& operators)
{
	std::vector<Serialization> stk;
	std::vector<analyzer::Query::Instruction>::const_iterator
		ii = val.instructions().begin(), ie = val.instructions().end();
	for (; ii != ie; ++ii)
	{
		switch (ii->opCode())
		{
			case analyzer::Query::Instruction::MetaData:
			{
				stk.push_back( Serialization());
				Serializer::serializeStructMember( stk.back(), "metadata", val.metadata( ii->idx()));
				break;
			}
			case analyzer::Query::Instruction::Term:
			{
				stk.push_back( Serialization());
				Serializer::serializeStructMember( stk.back(), "term", val.term( ii->idx()));
				break;
			}
			case analyzer::Query::Instruction::Operator:
			{
				Serialization opres;
				opres.pushName( "op");
				const QueryAnalyzerStruct::Operator& op = operators[ ii->idx()];
				Serializer::serializeStructMember( opres, "name", op.name);
				if (op.range) Serializer::serializeStructMember( opres, "range", (ValueVariant::IntType)op.range);
				if (op.cardinality) Serializer::serializeStructMember( opres, "cardinality", (ValueVariant::UIntType)op.cardinality);
				opres.pushName( "arg");
				opres.pushOpen();
				if (ii->nofOperands() > stk.size()) throw strus::runtime_error(_TXT("number of query analyzer expression operands out of range"));
				std::size_t si = stk.size() - ii->nofOperands(), se = stk.size();
				for (; si != se; ++si)
				{
					opres.pushOpen();
					opres.append( stk[si]);
					opres.pushClose();
				}
				opres.pushClose();
				stk.resize( stk.size() - ii->nofOperands());
				stk.push_back( opres);
				break;
			}
		}
	}
}
static void serialize_positional( Serialization& result, const analyzer::Query& val, const std::vector<QueryAnalyzerStruct::Operator>& operators)
{
	std::vector<Serialization> stk;
	std::vector<analyzer::Query::Instruction>::const_iterator
		ii = val.instructions().begin(), ie = val.instructions().end();
	for (; ii != ie; ++ii)
	{
		switch (ii->opCode())
		{
			case analyzer::Query::Instruction::MetaData:
			{
				stk.push_back( Serialization());
				serialize_positional( stk.back(), val.metadata( ii->idx()));
				break;
			}
			case analyzer::Query::Instruction::Term:
			{
				stk.push_back( Serialization());
				serialize_positional( stk.back(), val.term( ii->idx()));
				break;
			}
			case analyzer::Query::Instruction::Operator:
			{
				Serialization opres;
				opres.pushName( "op");
				const QueryAnalyzerStruct::Operator& op = operators[ ii->idx()];

				Serializer::serialize( opres, op.name);
				if (op.range) Serializer::serialize( opres, (ValueVariant::IntType)op.range);
				if (op.cardinality) Serializer::serialize( opres, (ValueVariant::UIntType)op.cardinality);
				
				if (ii->nofOperands() > stk.size()) throw strus::runtime_error(_TXT("number of query analyzer expression operands out of range"));
				std::size_t si = stk.size() - ii->nofOperands(), se = stk.size();
				for (; si != se; ++si)
				{
					opres.pushOpen();
					opres.append( stk[si]);
					opres.pushClose();
				}
				stk.resize( stk.size() - ii->nofOperands());
				stk.push_back( opres);
				break;
			}
		}
	}
}
void Serializer::serialize( Serialization& result, const analyzer::Query& val, const std::vector<QueryAnalyzerStruct::Operator>& operators, bool labeled)
{
	if (labeled)
	{
		serialize_labeled( result, val, operators);
	}
	else
	{
		serialize_positional( result, val, operators);
	}
}
void Serializer::serialize( Serialization& result, const bindings::DocumentFrequencyChange& val)
{
	serializeStructMember( result, "type", val.type());
	serializeStructMember( result, "value", val.value());
	serializeStructMember( result, "increment", (ValueVariant::IntType)val.increment());
}
void Serializer::serialize( Serialization& result, const bindings::StatisticsMessage& val)
{
	serializeStructMember( result, "dflist", val.dflist());
	serializeStructMember( result, "nofdocs", (ValueVariant::IntType)val.nofdocs());
}
void Serializer::serialize( Serialization& result, const ResultDocument& val)
{
	serializeStructMember( result, "docno", (ValueVariant::IntType)val.docno());
	serializeStructMember( result, "weight", val.weight());
	serializeStructMember( result, "summary", val.summaryElements());
}
void Serializer::serialize( Serialization& result, const QueryResult& val)
{
	serializeStructMember( result, "pass", (ValueVariant::UIntType)val.evaluationPass());
	serializeStructMember( result, "nofranked", (ValueVariant::UIntType)val.nofRanked());
	serializeStructMember( result, "nofvisited", (ValueVariant::UIntType)val.nofVisited());
	serializeStructMember( result, "ranks", val.ranks());
}
void Serializer::serialize( Serialization& result, const std::vector<VectorStorageSearchInterface::Result>& val)
{
	serializeArray( result, val);
}
void Serializer::serialize( Serialization& result, const std::vector<analyzer::Term>& val)
{
	serializeArray( result, val);
}
void Serializer::serialize( Serialization& result, const std::vector<std::string>& val)
{
	serializeArray( result, val);
}
void Serializer::serialize( Serialization& result, const std::vector<int>& val)
{
	serializeIntArray( result, val);
}
void Serializer::serialize( Serialization& result, const std::vector<double>& val)
{
	serializeArray( result, val);
}
void Serializer::serialize( Serialization& result, const std::vector<analyzer::MetaData>& val)
{
	serializeArray( result, val);
}
void Serializer::serialize( Serialization& result, const std::vector<analyzer::Attribute>& val)
{
	serializeArray( result, val);
}
void Serializer::serialize( Serialization& result, const std::vector<StatisticsViewerInterface::DocumentFrequencyChange>& val)
{
	serializeArray( result, val);
}
void Serializer::serialize( Serialization& result, const std::vector<strus::SummaryElement>& val)
{
	serializeArray( result, val);
}
void Serializer::serialize( Serialization& result, const std::vector<ResultDocument>& val)
{
	serializeArray( result, val);
}


