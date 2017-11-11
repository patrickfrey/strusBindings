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

bool Serializer::serialize_nothrow( papuga_Serialization* result, const NumericVariant& val, papuga_ErrorCode& errcode)
{
	switch (val.type)
	{
		case NumericVariant::Null:
			return papuga_Serialization_pushValue_void( result);
		case NumericVariant::Int:
			return papuga_Serialization_pushValue_int( result, val.variant.Int);
		case NumericVariant::UInt:
			return papuga_Serialization_pushValue_int( result, val.variant.UInt);
		case NumericVariant::Float:
			return papuga_Serialization_pushValue_double( result, val.variant.Float);
	}
	errcode = papuga_TypeError;
	return false;
}

bool Serializer::serialize_nothrow( papuga_Serialization* result, const analyzer::QueryTerm& val, const char* variablename, papuga_ErrorCode& errcode)
{
	bool rt = true;
	rt &= serializeStructMember( result, "type", val.type(), errcode);
	rt &= serializeStructMember( result, "value", val.value(), errcode);
	if ((papuga_Int)val.len() > 1) rt &= serializeStructMember( result, "len", (papuga_Int)val.len(), errcode);
	if (variablename) rt &= serializeStructMember( result, "variable", variablename, errcode);
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const analyzer::DocumentTerm& val, papuga_ErrorCode& errcode)
{
	bool rt = true;
	rt &= serializeStructMember( result, "type", val.type(), errcode);
	rt &= serializeStructMember( result, "value", val.value(), errcode);
	if ((papuga_Int)val.pos()) rt &= serializeStructMember( result, "pos", (papuga_Int)val.pos(), errcode);
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const analyzer::DocumentAttribute& val, papuga_ErrorCode& errcode)
{
	bool rt = true;
	rt &= serializeStructMember( result, val.name().c_str(), val.value(), errcode);
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const analyzer::DocumentMetaData& val, papuga_ErrorCode& errcode)
{
	bool rt = true;
	rt &= serializeStructMember( result, val.name().c_str(), val.value(), errcode);
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const analyzer::DocumentClass& val, papuga_ErrorCode& errcode)
{
	bool rt = true;
	if (val.defined())
	{
		rt &= serialize_nothrow( result, val.mimeType(), errcode);
		rt &= serialize_nothrow( result, val.encoding(), errcode);
		rt &= serialize_nothrow( result, val.scheme(), errcode);
	}
	else
	{
		rt &= papuga_Serialization_pushValue_void( result);
	}
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const VectorQueryResult& val, papuga_ErrorCode& errcode)
{
	bool rt = true;
	rt &= serializeStructMember( result, "featidx", (papuga_Int)val.featidx(), errcode);
	rt &= serializeStructMember( result, "weight", val.weight(), errcode);
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const SummaryElement& val, papuga_ErrorCode& errcode)
{
	bool rt = true;
	rt &= serializeStructMember( result, "name", val.name(), errcode);
	rt &= serializeStructMember( result, "value", val.value(), errcode);
	rt &= serializeStructMember( result, "weight", val.weight(), errcode);
	rt &= serializeStructMember( result, "index", (papuga_Int)val.index(), errcode);
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const analyzer::Document& val, papuga_ErrorCode& errcode)
{
	bool rt = true;
	rt &= serializeStructMember( result, "doctype", val.subDocumentTypeName(), errcode);
	rt &= serializeStructMember( result, "metadata", val.metadata(), errcode);
	rt &= serializeStructMember( result, "attribute", val.attributes(), errcode);
	rt &= serializeStructMember( result, "searchindex", val.searchIndexTerms(), errcode);
	rt &= serializeStructMember( result, "forwardindex", val.forwardIndexTerms(), errcode);
	return rt;
}
static const char* getTermExpressionVariableName(
		const TermExpression& expr,
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
bool Serializer::serialize_nothrow( papuga_Serialization* result, const TermExpression& val, papuga_ErrorCode& errcode)
{
	try
	{
		bool rt = true;
		const analyzer::QueryTermExpression& expr = val.expression();
		std::vector<papuga_Serialization*> stk;

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
					rt &= Serializer::serialize_nothrow( ser, expr.term( ii->idx()), variablename, errcode);
					break;
				}
				case analyzer::QueryTermExpression::Instruction::Operator:
				{
					if (!val.isOperator( ii->idx())) return false;

					papuga_Serialization* ser = papuga_Allocator_alloc_Serialization( result->allocator);
					if (!ser) return false;

					const TermExpression::Operator& op = val.operatorStruct( ii->idx());
					const char* variablename = getTermExpressionVariableName( val, ii, ie);
	
					rt &= Serializer::serializeStructMember( ser, "op", op.name, errcode);
					if (op.range) rt &= Serializer::serializeStructMember( ser, "range", (papuga_Int)op.range, errcode);
					if (op.cardinality) rt &= Serializer::serializeStructMember( ser, "cardinality", (papuga_Int)op.cardinality, errcode);
	
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
						rt &= Serializer::serializeStructMember( ser, "variable", variablename, errcode);
					}
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
		return rt;
	}
	catch (const std::bad_alloc&)
	{
		return false;
	}
}

bool Serializer::serialize_nothrow( papuga_Serialization* result, const MetaDataComparison& val, papuga_ErrorCode& errcode)
{
	bool rt = true;
	papuga_ErrorCode err = papuga_Ok;
	papuga_ValueVariant buf;
	papuga_ValueVariant termval;
	papuga_init_ValueVariant_string( &termval, val.term()->value().c_str(), val.term()->value().size());
	papuga_ValueVariant* num = papuga_ValueVariant_tonumeric( &termval, &buf, &err);
	rt &= Serializer::serializeStructMember( result, "op", val.cmpop(), errcode);
	rt &= Serializer::serializeStructMember( result, "name", val.term()->type(), errcode);
	if (num)
	{
		rt &= Serializer::serializeStructMember( result, "value", *num, errcode);
	}
	else
	{
		rt &= Serializer::serializeStructMember( result, "value", val.term()->value(), errcode);
	}
	return rt;
}

bool Serializer::serialize_nothrow( papuga_Serialization* result, const MetaDataExpression& val, papuga_ErrorCode& errcode)
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
						errcode = papuga_TypeError;
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
									errcode = papuga_NofArgsError;
									return false;
								}
								// We check that all operands of an OR are atomic terms (CNF):
								std::vector<MetaDataComparison>::iterator
									ci = cmplist.end() - termc + 1, ce = cmplist.end();
								for (; ci != ce; ++ci)
								{
									ci->setNewGroup( false);
								}
							}
						}
						termc = 0;
					}
					else
					{
						errcode = papuga_TypeError;
						return false;
					}
					break;
				}
			}
		}
		// Serialize the CNF:
		std::vector<MetaDataComparison>::const_iterator ci = cmplist.begin(), ce = cmplist.end();
		for (; ci != ce; ++ci)
		{
			rt &= papuga_Serialization_pushOpen( result);

			std::vector<MetaDataComparison>::const_iterator cn = ci;
			unsigned int argcnt = 1;
			for (++cn; cn != ce && !cn->newGroup(); ++cn,++argcnt){}
			if (argcnt > 1)
			{
				for (; ci != cn; ++ci)
				{
					rt &= papuga_Serialization_pushOpen( result);
					rt &= serialize_nothrow( result, *ci, errcode);
					rt &= papuga_Serialization_pushClose( result);
				}
				--ci;
			}
			else
			{
				rt &= serialize_nothrow( result, *ci, errcode);
			}
			rt &= papuga_Serialization_pushClose( result);
		}
		if (!rt)
		{
			errcode = papuga_NoMemError;
		}
		return rt;
	}
	catch (const std::bad_alloc&)
	{
		errcode = papuga_NoMemError;
		return false;
	}
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const TermStatisticsChange& val, papuga_ErrorCode& errcode)
{
	bool rt = true;
	const char* type = papuga_Allocator_copy_charp( result->allocator, val.type());
	rt &= serializeStructMember( result, "type", type, errcode);
	const char* value = papuga_Allocator_copy_charp( result->allocator, val.value());
	rt &= serializeStructMember( result, "value", value, errcode);
	rt &= serializeStructMember( result, "increment", (papuga_Int)val.increment(), errcode);
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, StatisticsViewerInterface& val, papuga_ErrorCode& errcode)
{
	bool rt = true;

	rt &= papuga_Serialization_pushName_charp( result, "dfchange");
	rt &= papuga_Serialization_pushOpen( result);

	TermStatisticsChange rec;
	while (val.nextDfChange( rec))
	{
		rt &= serializeArrayElement( result, rec, errcode);
	}
	rt &= papuga_Serialization_pushClose( result);

	rt &= papuga_Serialization_pushName_charp( result, "nofdocs");
	rt &= papuga_Serialization_pushValue_int( result, val.nofDocumentsInsertedChange());
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const ResultDocument& val, papuga_ErrorCode& errcode)
{
	bool rt = true;
	rt &= serializeStructMember( result, "docno", (papuga_Int)val.docno(), errcode);
	rt &= serializeStructMember( result, "weight", val.weight(), errcode);
	rt &= serializeStructMember( result, "summary", val.summaryElements(), errcode);
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const QueryResult& val, papuga_ErrorCode& errcode)
{
	bool rt = true;
	rt &= serializeStructMember( result, "pass", (papuga_Int)val.evaluationPass(), errcode);
	rt &= serializeStructMember( result, "nofranked", (papuga_Int)val.nofRanked(), errcode);
	rt &= serializeStructMember( result, "nofvisited", (papuga_Int)val.nofVisited(), errcode);
	rt &= serializeStructMember( result, "ranks", val.ranks(), errcode);
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const ConfigurationItemList& val, papuga_ErrorCode& errcode)
{
	bool rt = true;
	ConfigurationItemList::const_iterator ci = val.begin(), ce = val.end();
	for (; ci != ce; ++ci)
	{
		rt &= papuga_Serialization_pushName_string( result, ci->first.c_str(), ci->first.size());
		rt &= papuga_Serialization_pushValue_string( result, ci->second.c_str(), ci->second.size());
	}
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<VectorQueryResult>& val, papuga_ErrorCode& errcode)
{
	return serializeArray( result, val, errcode);
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<std::string>& val, papuga_ErrorCode& errcode)
{
	return serializeArray( result, val, errcode);
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<int>& val, papuga_ErrorCode& errcode)
{
	return serializeIntArray( result, val, errcode);
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<double>& val, papuga_ErrorCode& errcode)
{
	return serializeArray( result, val, errcode);
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<analyzer::DocumentTerm>& val, papuga_ErrorCode& errcode)
{
	return serializeArray( result, val, errcode);
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<analyzer::DocumentMetaData>& val, papuga_ErrorCode& errcode)
{
	bool rt = true;
	std::vector<analyzer::DocumentMetaData>::const_iterator vi = val.begin(), ve = val.end();
	for (; vi != ve; ++vi)
	{
		rt &= serializeStructMember( result, vi->name().c_str(), vi->value(), errcode);
	}
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<analyzer::DocumentAttribute>& val, papuga_ErrorCode& errcode)
{
	bool rt = true;
	std::vector<analyzer::DocumentAttribute>::const_iterator vi = val.begin(), ve = val.end();
	for (; vi != ve; ++vi)
	{
		rt &= serializeStructMember( result, vi->name().c_str(), vi->value(), errcode);
	}
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<strus::SummaryElement>& val, papuga_ErrorCode& errcode)
{
	return serializeArray( result, val, errcode);
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<ResultDocument>& val, papuga_ErrorCode& errcode)
{
	return serializeArray( result, val, errcode);
}


