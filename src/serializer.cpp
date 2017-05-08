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
#include "papuga/valueVariant.h"
#include "papugaSerialization.hpp"
#include "internationalization.hpp"

using namespace strus;
using namespace strus::bindings;
using namespace papuga;

bool Serializer::serialize_nothrow( papuga_Serialization* result, const NumericVariant& val)
{
	switch (val.type)
	{
		case NumericVariant::Null:
			return papuga_Serialization_pushValue_void( result);
		case NumericVariant::Int:
			return papuga_Serialization_pushValue_int( result, val.variant.Int);
		case NumericVariant::UInt:
			return papuga_Serialization_pushValue_uint( result, val.variant.UInt);
		case NumericVariant::Float:
			return papuga_Serialization_pushValue_double( result, val.variant.Float);
	}
	throw strus::runtime_error(_TXT("unknown numeric type passed to serialization"));
}

static bool serialize_positional( papuga_Serialization* result, const analyzer::Term& val)
{
	bool rt = true;
	rt &= Serializer::serialize_nothrow( result, val.type());
	rt &= Serializer::serialize_nothrow( result, val.value());
	rt &= Serializer::serialize_nothrow( result, (papuga_UIntType)val.pos());
	rt &= Serializer::serialize_nothrow( result, (papuga_UIntType)val.len());
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const analyzer::Term& val)
{
	bool rt = true;
	rt &= serializeStructMember( result, "type", val.type());
	rt &= serializeStructMember( result, "value", val.value());
	rt &= serializeStructMember( result, "pos", (papuga_UIntType)val.pos());
	rt &= serializeStructMember( result, "len", (papuga_UIntType)val.len());
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const analyzer::Attribute& val)
{
	bool rt = true;
	rt &= serializeStructMember( result, "name", val.name());
	rt &= serializeStructMember( result, "value", val.value());
	return rt;
}
static bool serialize_positional( papuga_Serialization* result, const analyzer::MetaData& val)
{
	bool rt = true;
	rt &= Serializer::serialize_nothrow( result, val.name());
	rt &= Serializer::serialize_nothrow( result, val.value());
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const analyzer::MetaData& val)
{
	bool rt = true;
	rt &= serializeStructMember( result, "name", val.name());
	rt &= serializeStructMember( result, "value", val.value());
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const analyzer::DocumentClass& val)
{
	bool rt = true;
	if (!val.mimeType().empty()) rt &= serializeStructMember( result, "mimetype", val.mimeType());
	if (!val.encoding().empty()) rt &= serializeStructMember( result, "encoding", val.encoding());
	if (!val.scheme().empty()) rt &= serializeStructMember( result, "scheme", val.scheme());
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const VectorStorageSearchInterface::Result& val)
{
	bool rt = true;
	rt &= serializeStructMember( result, "featidx", (papuga_IntType)val.featidx());
	rt &= serializeStructMember( result, "weight", val.weight());
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const SummaryElement& val)
{
	bool rt = true;
	rt &= serializeStructMember( result, "name", val.name());
	rt &= serializeStructMember( result, "value", val.value());
	rt &= serializeStructMember( result, "weight", val.weight());
	rt &= serializeStructMember( result, "index", (papuga_IntType)val.index());
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const analyzer::Document& val)
{
	bool rt = true;
	rt &= serializeStructMember( result, "doctype", val.subDocumentTypeName());
	rt &= serializeStructMember( result, "metadata", val.metadata());
	rt &= serializeStructMember( result, "attributes", val.attributes());
	rt &= serializeStructMember( result, "searchindex", val.searchIndexTerms());
	rt &= serializeStructMember( result, "forwardindex", val.forwardIndexTerms());
	return rt;
}
static bool serialize_labeled( papuga_Serialization* result, const analyzer::Query& val, const std::vector<QueryAnalyzerStruct::Operator>& operators)
{
	bool rt = true;
	std::vector<papuga::Serialization> stk;
	std::vector<analyzer::Query::Instruction>::const_iterator
		ii = val.instructions().begin(), ie = val.instructions().end();
	for (; ii != ie && rt; ++ii)
	{
		switch (ii->opCode())
		{
			case analyzer::Query::Instruction::MetaData:
			{
				stk.push_back( Serialization());
				rt &= Serializer::serializeStructMember( stk.back().cstruct(), "metadata", val.metadata( ii->idx()));
				break;
			}
			case analyzer::Query::Instruction::Term:
			{
				stk.push_back( Serialization());
				rt &= Serializer::serializeStructMember( stk.back().cstruct(), "term", val.term( ii->idx()));
				break;
			}
			case analyzer::Query::Instruction::Operator:
			{
				Serialization opres;
				rt &= papuga_Serialization_pushName_charp( opres.cstruct(), "op");
				rt &= papuga_Serialization_pushOpen( opres.cstruct());

				const QueryAnalyzerStruct::Operator& op = operators[ ii->idx()];
				rt &= Serializer::serializeStructMember( opres.cstruct(), "name", op.name);
				if (op.range) rt &= Serializer::serializeStructMember( opres.cstruct(), "range", (papuga_IntType)op.range);
				if (op.cardinality) rt &= Serializer::serializeStructMember( opres.cstruct(), "cardinality", (papuga_UIntType)op.cardinality);

				rt &= papuga_Serialization_pushName_charp( opres.cstruct(), "arg");
				rt &= papuga_Serialization_pushOpen( opres.cstruct());
				if (ii->nofOperands() > stk.size()) throw strus::runtime_error(_TXT("number of query analyzer expression operands out of range"));
				std::size_t si = stk.size() - ii->nofOperands(), se = stk.size();
				for (; si != se; ++si)
				{
					rt &= papuga_Serialization_pushOpen( opres.cstruct());
					papuga_Serialization_append( opres.cstruct(), stk[si].cstruct());
					rt &= papuga_Serialization_pushClose( opres.cstruct());
				}
				rt &= papuga_Serialization_pushClose( opres.cstruct());
				rt &= papuga_Serialization_pushClose( opres.cstruct());
				stk.resize( stk.size() - ii->nofOperands());
				stk.push_back( opres);
				break;
			}
		}
	}
	return rt;
}
static bool serialize_positional( papuga_Serialization* result, const analyzer::Query& val, const std::vector<QueryAnalyzerStruct::Operator>& operators)
{
	bool rt = true;
	std::vector<Serialization> stk;
	std::vector<analyzer::Query::Instruction>::const_iterator
		ii = val.instructions().begin(), ie = val.instructions().end();
	for (; ii != ie && rt; ++ii)
	{
		switch (ii->opCode())
		{
			case analyzer::Query::Instruction::MetaData:
			{
				stk.push_back( Serialization());
				rt &= serialize_positional( stk.back().cstruct(), val.metadata( ii->idx()));
				break;
			}
			case analyzer::Query::Instruction::Term:
			{
				stk.push_back( Serialization());
				rt &= serialize_positional( stk.back().cstruct(), val.term( ii->idx()));
				break;
			}
			case analyzer::Query::Instruction::Operator:
			{
				Serialization opres;
				const QueryAnalyzerStruct::Operator& op = operators[ ii->idx()];

				rt &= Serializer::serialize_nothrow( opres.cstruct(), op.name);
				if (op.range) rt &= Serializer::serialize_nothrow( opres.cstruct(), (papuga_IntType)op.range);
				if (op.cardinality) rt &= Serializer::serialize_nothrow( opres.cstruct(), (papuga_UIntType)op.cardinality);
				
				if (ii->nofOperands() > stk.size()) throw strus::runtime_error(_TXT("number of query analyzer expression operands out of range"));
				std::size_t si = stk.size() - ii->nofOperands(), se = stk.size();
				for (; si != se; ++si)
				{
					rt &= papuga_Serialization_pushOpen( opres.cstruct());
					papuga_Serialization_append( opres.cstruct(), stk[si].cstruct());
					rt &= papuga_Serialization_pushClose( opres.cstruct());
				}
				stk.resize( stk.size() - ii->nofOperands());
				stk.push_back( opres);
				break;
			}
		}
	}
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const QueryAnalyzerImpl::AnalyzeResult& val)
{
	if (val.labeledOutput())
	{
		return serialize_labeled( result, *val.query(), val.operators());
	}
	else
	{
		return serialize_positional( result, *val.query(), val.operators());
	}
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const StatisticsViewerInterface::DocumentFrequencyChange& val)
{
	bool rt = true;
	rt &= serializeStructMember( result, "type", val.type());
	rt &= serializeStructMember( result, "value", val.value());
	rt &= serializeStructMember( result, "increment", (papuga_IntType)val.increment());
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const ResultDocument& val)
{
	bool rt = true;
	rt &= serializeStructMember( result, "docno", (papuga_IntType)val.docno());
	rt &= serializeStructMember( result, "weight", val.weight());
	rt &= serializeStructMember( result, "summary", val.summaryElements());
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const QueryResult& val)
{
	bool rt = true;
	rt &= serializeStructMember( result, "pass", (papuga_UIntType)val.evaluationPass());
	rt &= serializeStructMember( result, "nofranked", (papuga_UIntType)val.nofRanked());
	rt &= serializeStructMember( result, "nofvisited", (papuga_UIntType)val.nofVisited());
	rt &= serializeStructMember( result, "ranks", val.ranks());
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const ConfigurationItemList& val)
{
	bool rt = true;
	ConfigurationItemList::const_iterator ci = val.begin(), ce = val.end();
	for (; ci != ce; ++ci)
	{
		rt &= papuga_Serialization_pushName_string( result, ci->first.c_str(), ci->first.size());
		rt &= papuga_Serialization_pushValue_string( result, ci->first.c_str(), ci->first.size());
	}
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<VectorStorageSearchInterface::Result>& val)
{
	return serializeArray( result, val);
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<analyzer::Term>& val)
{
	return serializeArray( result, val);
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<std::string>& val)
{
	return serializeArray( result, val);
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<int>& val)
{
	return serializeIntArray( result, val);
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<double>& val)
{
	return serializeArray( result, val);
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<analyzer::MetaData>& val)
{
	return serializeArray( result, val);
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<analyzer::Attribute>& val)
{
	return serializeArray( result, val);
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<StatisticsViewerInterface::DocumentFrequencyChange>& val)
{
	return serializeArray( result, val);
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<strus::SummaryElement>& val)
{
	return serializeArray( result, val);
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<ResultDocument>& val)
{
	return serializeArray( result, val);
}


