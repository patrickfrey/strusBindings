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
#include "papuga/serialization.h"
#include "internationalization.hpp"
#include "papuga/allocator.h"

using namespace strus;
using namespace strus::bindings;

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
	throw strus::runtime_error( "%s", _TXT("unknown numeric type passed to serialization"));
}

bool Serializer::serialize_nothrow( papuga_Serialization* result, const analyzer::QueryTerm& val, const char* variablename)
{
	bool rt = true;
	rt &= papuga_Serialization_pushOpen( result);
	rt &= serializeStructMember( result, "type", val.type());
	rt &= serializeStructMember( result, "value", val.value());
	if ((papuga_UInt)val.len() > 1) rt &= serializeStructMember( result, "len", (papuga_UInt)val.len());
	if (variablename) rt &= serializeStructMember( result, "variable", variablename);
	rt &= papuga_Serialization_pushClose( result);
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const analyzer::DocumentTerm& val)
{
	bool rt = true;
	rt &= papuga_Serialization_pushOpen( result);
	rt &= serializeStructMember( result, "type", val.type());
	rt &= serializeStructMember( result, "value", val.value());
	if ((papuga_UInt)val.pos()) rt &= serializeStructMember( result, "pos", (papuga_UInt)val.pos());
	rt &= papuga_Serialization_pushClose( result);
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const analyzer::DocumentAttribute& val)
{
	bool rt = true;
	rt &= serializeStructMember( result, val.name().c_str(), val.value());
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const analyzer::DocumentMetaData& val)
{
	bool rt = true;
	rt &= serializeStructMember( result, val.name().c_str(), val.value());
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const analyzer::DocumentClass& val)
{
	bool rt = true;
	if (val.defined())
	{
		rt &= papuga_Serialization_pushOpen( result);
		if (!val.mimeType().empty()) rt &= serializeStructMember( result, "mimetype", val.mimeType());
		if (!val.encoding().empty()) rt &= serializeStructMember( result, "encoding", val.encoding());
		if (!val.scheme().empty()) rt &= serializeStructMember( result, "scheme", val.scheme());
		rt &= papuga_Serialization_pushClose( result);
	}
	else
	{
		rt &= papuga_Serialization_pushValue_void( result);
	}
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const VectorStorageSearchInterface::Result& val)
{
	bool rt = true;
	rt &= papuga_Serialization_pushOpen( result);
	rt &= serializeStructMember( result, "featidx", (papuga_Int)val.featidx());
	rt &= serializeStructMember( result, "weight", val.weight());
	rt &= papuga_Serialization_pushClose( result);
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const SummaryElement& val)
{
	bool rt = true;
	rt &= papuga_Serialization_pushOpen( result);
	rt &= serializeStructMember( result, "name", val.name());
	rt &= serializeStructMember( result, "value", val.value());
	rt &= serializeStructMember( result, "weight", val.weight());
	rt &= serializeStructMember( result, "index", (papuga_Int)val.index());
	rt &= papuga_Serialization_pushClose( result);
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const analyzer::Document& val)
{
	bool rt = true;
	rt &= papuga_Serialization_pushOpen( result);
	rt &= serializeStructMember( result, "doctype", val.subDocumentTypeName());
	rt &= serializeStructMember( result, "metadata", val.metadata());
	rt &= serializeStructMember( result, "attribute", val.attributes());
	rt &= serializeStructMember( result, "searchindex", val.searchIndexTerms());
	rt &= serializeStructMember( result, "forwardindex", val.forwardIndexTerms());
	rt &= papuga_Serialization_pushClose( result);
	return rt;
}
static const char* getTermExpressionVariableName( const TermExpression& expr,
				std::vector<analyzer::QueryTermExpression::Instruction>::const_iterator ii,
				const std::vector<analyzer::QueryTermExpression::Instruction>::const_iterator& ie)
{
	++ii;
	if (ii != ie
		&& ii->opCode() == analyzer::QueryTermExpression::Instruction::Operator
		&& expr.isVariable( ii->idx()))
	{
		return expr.variableName( ii->idx()).c_str();
	}
	return 0;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const TermExpression& val)
{
	try
	{
		bool rt = true;
		const analyzer::QueryTermExpression& expr = val.expression();
		std::vector<papuga_Serialization*> stk;

		rt &= papuga_Serialization_pushOpen( result);
		std::vector<analyzer::QueryTermExpression::Instruction>::const_iterator
			ii = expr.instructions().begin(), ie = expr.instructions().end();
		for (; ii != ie && rt; ++ii)
		{
			switch (ii->opCode())
			{
				case analyzer::QueryTermExpression::Instruction::Term:
				{
					papuga_Serialization* ser = papuga_Allocator_alloc_Serialization( result->allocator);
					if (!ser) return false;
					stk.push_back( ser);
					const char* variablename = getTermExpressionVariableName( val, ii, ie);
					rt &= Serializer::serialize_nothrow( ser, expr.term( ii->idx()), variablename);
					break;
				}
				case analyzer::QueryTermExpression::Instruction::Operator:
				{
					if (!val.isOperator( ii->idx())) return false;

					papuga_Serialization* ser = papuga_Allocator_alloc_Serialization( result->allocator);
					if (!ser) return false;

					rt &= papuga_Serialization_pushOpen( ser);
					const TermExpression::Operator& op = val.operatorStruct( ii->idx());
					const char* variablename = getTermExpressionVariableName( val, ii, ie);
	
					rt &= Serializer::serializeStructMember( ser, "op", op.name);
					if (op.range) rt &= Serializer::serializeStructMember( ser, "range", (papuga_Int)op.range);
					if (op.cardinality) rt &= Serializer::serializeStructMember( ser, "cardinality", (papuga_UInt)op.cardinality);
	
					rt &= papuga_Serialization_pushName_charp( ser, "arg");
					rt &= papuga_Serialization_pushOpen( ser);
					if (ii->nofOperands() > stk.size()) throw strus::runtime_error( "%s", _TXT("number of query analyzer expression operands out of range"));
					std::size_t si = stk.size() - ii->nofOperands(), se = stk.size();
					for (; si != se; ++si)
					{
						papuga_Serialization_pushValue_serialization( ser, stk[si]);
					}
					rt &= papuga_Serialization_pushClose( ser);//... end arg
					if (variablename)
					{
						rt &= Serializer::serializeStructMember( ser, "variable", variablename);
					}
					rt &= papuga_Serialization_pushClose( ser);//... end operator
					stk.resize( stk.size() - ii->nofOperands());
					stk.push_back( ser);
					break;
				}
			}
		}
		// Collect list of results from the stack:
		std::vector<papuga_Serialization*>::const_iterator si = stk.begin(), se = stk.end();
		for (; si != se; ++si)
		{
			rt &= papuga_Serialization_pushValue_serialization( result, *si);
		}
		rt &= papuga_Serialization_pushClose( result);
		return rt;
	}
	catch (const std::bad_alloc&)
	{
		return false;
	}
}
struct MetaDataComparison
{
	const char* cmpop;
	const analyzer::QueryTerm* term;
	bool newGroup;

	MetaDataComparison( const char* cmpop_, const analyzer::QueryTerm* term_, bool newGroup_)
		:cmpop(cmpop_),term(term_),newGroup(newGroup_){}
	MetaDataComparison( const MetaDataComparison& o)
		:cmpop(o.cmpop),term(o.term),newGroup(o.newGroup){}

	bool serialize( papuga_Serialization* result) const
	{
		bool rt = true;
		papuga_ErrorCode err = papuga_Ok;
		papuga_ValueVariant buf;
		papuga_ValueVariant termval;
		papuga_init_ValueVariant_string( &termval, term->value().c_str(), term->value().size());
		papuga_ValueVariant* num = papuga_ValueVariant_tonumeric( &termval, &buf, &err);
		rt &= papuga_Serialization_pushOpen( result);
		rt &= Serializer::serializeStructMember( result, "op", cmpop);
		rt &= Serializer::serializeStructMember( result, "name", term->type());
		if (num)
		{
			rt &= Serializer::serializeStructMember( result, "value", *num);
		}
		else
		{
			rt &= Serializer::serializeStructMember( result, "value", term->value());
		}
		rt &= papuga_Serialization_pushClose( result);
		return rt;
	}
};
bool Serializer::serialize_nothrow( papuga_Serialization* result, const MetaDataExpression& val, papuga_ErrorCode& err)
{
	try
	{
		bool rt = true;
		const analyzer::QueryTermExpression& expr = val.expression();
		unsigned int termc = 0;

		// Build a simpler data structure of a CNF (conjunctive normal form):
		std::vector<MetaDataComparison> cmplist;
		std::vector<analyzer::QueryTermExpression::Instruction>::const_iterator
			ii = expr.instructions().begin(), ie = expr.instructions().end();
		for (; ii != ie && rt; ++ii)
		{
			switch (ii->opCode())
			{
				case analyzer::QueryTermExpression::Instruction::Term:
				{
					const analyzer::QueryTerm* term = &expr.term( ii->idx());
					++termc;
					++ii;
					if (ii != ie
						&& ii->opCode() == analyzer::QueryTermExpression::Instruction::Operator
						&& val.isCompareOp( ii->idx()))
					{
						const char* opname = MetaDataExpression::compareOp( ii->idx());
						cmplist.push_back( MetaDataComparison( opname, term, true));
					}
					else
					{
						err = papuga_TypeError;
						return false;
					}
					break;
				}
				case analyzer::QueryTermExpression::Instruction::Operator:
				{
					if (val.isBooleanOp( ii->idx()))
					{
						MetaDataExpression::BooleanOp bop = val.booleanOp( ii->idx());
						if (bop == MetaDataExpression::OperatorOR)
						{
							if (termc > 1)
							{
								if (termc > ii->nofOperands())
								{
									err = papuga_NofArgsError;
									return false;
								}
								// We check that all operands of an OR are atomic terms (CNF):
								std::vector<MetaDataComparison>::iterator
									ci = cmplist.end() - termc + 1, ce = cmplist.end();
								for (; ci != ce; ++ci)
								{
									ci->newGroup = false;
								}
							}
						}
						termc = 0;
					}
					else
					{
						err = papuga_TypeError;
						return false;
					}
					break;
				}
			}
		}
		// Serialize the CNF:
		rt &= papuga_Serialization_pushOpen( result);
		std::vector<MetaDataComparison>::const_iterator ci = cmplist.begin(), ce = cmplist.end();
		for (; ci != ce; ++ci)
		{
			std::vector<MetaDataComparison>::const_iterator cn = ci;
			unsigned int argcnt = 1;
			for (++cn; cn != ce && !cn->newGroup; ++cn,++argcnt){}
			if (argcnt > 1)
			{
				rt &= papuga_Serialization_pushOpen( result);
				for (; ci != cn; ++ci)
				{
					rt &= ci->serialize( result);
				}
				--ci;
				rt &= papuga_Serialization_pushClose( result);
			}
			else
			{
				rt &= ci->serialize( result);
			}
			
		}
		rt &= papuga_Serialization_pushClose( result);
		if (!rt)
		{
			err = papuga_NoMemError;
		}
		return rt;
	}
	catch (const std::bad_alloc&)
	{
		err = papuga_NoMemError;
		return false;
	}
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const StatisticsViewerInterface::DocumentFrequencyChange& val, papuga_Allocator* allocator)
{
	bool rt = true;
	rt &= papuga_Serialization_pushOpen( result);
	const char* type = papuga_Allocator_copy_charp( allocator, val.type());
	rt &= serializeStructMember( result, "type", type);
	const char* value = papuga_Allocator_copy_charp( allocator, val.value());
	rt &= serializeStructMember( result, "value", value);
	rt &= serializeStructMember( result, "increment", (papuga_Int)val.increment());
	rt &= papuga_Serialization_pushClose( result);
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, StatisticsViewerInterface& val, papuga_Allocator* allocator)
{
	bool rt = true;

	rt &= papuga_Serialization_pushOpen( result);
	rt &= papuga_Serialization_pushName_charp( result, "dfchange");
	rt &= papuga_Serialization_pushOpen( result);

	DocumentFrequencyChange rec;
	while (val.nextDfChange( rec))
	{
		rt &= Serializer::serialize_nothrow( result, rec, allocator);
	}
	rt &= papuga_Serialization_pushClose( result);

	rt &= papuga_Serialization_pushName_charp( result, "nofdocs");
	rt &= papuga_Serialization_pushValue_int( result, val.nofDocumentsInsertedChange());
	rt &= papuga_Serialization_pushClose( result);
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const ResultDocument& val)
{
	bool rt = true;
	rt &= papuga_Serialization_pushOpen( result);
	rt &= serializeStructMember( result, "docno", (papuga_Int)val.docno());
	rt &= serializeStructMember( result, "weight", val.weight());
	rt &= serializeStructMember( result, "summary", val.summaryElements());
	rt &= papuga_Serialization_pushClose( result);
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const QueryResult& val)
{
	bool rt = true;
	rt &= papuga_Serialization_pushOpen( result);
	rt &= serializeStructMember( result, "pass", (papuga_UInt)val.evaluationPass());
	rt &= serializeStructMember( result, "nofranked", (papuga_UInt)val.nofRanked());
	rt &= serializeStructMember( result, "nofvisited", (papuga_UInt)val.nofVisited());
	rt &= serializeStructMember( result, "ranks", val.ranks());
	rt &= papuga_Serialization_pushClose( result);
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const ConfigurationItemList& val)
{
	bool rt = true;
	rt &= papuga_Serialization_pushOpen( result);
	ConfigurationItemList::const_iterator ci = val.begin(), ce = val.end();
	for (; ci != ce; ++ci)
	{
		rt &= papuga_Serialization_pushName_string( result, ci->first.c_str(), ci->first.size());
		rt &= papuga_Serialization_pushValue_string( result, ci->second.c_str(), ci->second.size());
	}
	rt &= papuga_Serialization_pushClose( result);
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<VectorStorageSearchInterface::Result>& val)
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
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<analyzer::DocumentTerm>& val)
{
	return serializeArray( result, val);
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<analyzer::DocumentMetaData>& val)
{
	return serializeArray( result, val);
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<analyzer::DocumentAttribute>& val)
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


