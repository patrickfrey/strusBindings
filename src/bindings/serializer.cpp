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
#include "private/internationalization.hpp"
#include "papuga/allocator.h"
#include "strus/base/localErrorBuffer.hpp"
#include "strus/base/configParser.hpp"

using namespace strus;
using namespace strus::bindings;

bool Serializer::serialize_nothrow( papuga_Serialization* result, const NumericVariant& val, papuga_ErrorCode& errcode, bool deep)
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

bool Serializer::serialize_nothrow( papuga_Serialization* result, const analyzer::QueryTerm& val, const char* variablename, papuga_ErrorCode& errcode, bool deep)
{
	bool rt = true;
	rt &= serializeArrayElement( result, val.type(), errcode, deep);
	rt &= serializeArrayElement( result, val.value(), errcode, deep);
	if ((papuga_Int)val.len() > 1) rt &= serializeArrayElement( result, (papuga_Int)val.len(), errcode, deep);
	if (variablename) rt &= serializeArrayElement( result, variablename, errcode, deep);
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const analyzer::DocumentTerm& val, papuga_ErrorCode& errcode, bool deep)
{
	bool rt = true;
	rt &= serializeArrayElement( result, val.type(), errcode, deep);
	rt &= serializeArrayElement( result, val.value(), errcode, deep);
	if ((papuga_Int)val.pos()) rt &= serializeArrayElement( result, (papuga_Int)val.pos(), errcode, deep);
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const analyzer::DocumentAttribute& val, papuga_ErrorCode& errcode, bool deep)
{
	bool rt = true;
	rt &= serializeStructMember( result, val.name().c_str(), val.value(), errcode, deep);
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const analyzer::DocumentMetaData& val, papuga_ErrorCode& errcode, bool deep)
{
	bool rt = true;
	rt &= serializeStructMember( result, val.name().c_str(), val.value(), errcode, deep);
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, DocumentTermIteratorInterface* dtitr, papuga_ErrorCode& errcode, bool deep)
{
	bool rt = true;
	DocumentTermIteratorInterface::Term term;
	while (rt && dtitr->nextTerm( term))
	{
		rt &= papuga_Serialization_pushOpen_struct( result, StructIdTemplate<DocumentTermIteratorInterface::Term>::structid());
		rt &= serializeArrayElement( result, (int64_t)term.tf, errcode, deep);
		rt &= serializeArrayElement( result, (int64_t)term.firstpos, errcode, deep);
		rt &= serializeArrayElement( result, dtitr->termValue( term.termno), errcode, deep);
		rt &= papuga_Serialization_pushClose( result);
	}
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const analyzer::DocumentClass& val, papuga_ErrorCode& errcode, bool deep)
{
	bool rt = true;
	if (val.defined())
	{
		rt &= serializeArrayElement( result, val.mimeType(), errcode, deep);
		rt &= serializeArrayElement( result, val.encoding(), errcode, deep);
		rt &= serializeArrayElement( result, val.scheme(), errcode, deep);
	}
	else
	{
		rt &= papuga_Serialization_pushValue_void( result);
	}
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const VectorQueryResult& val, papuga_ErrorCode& errcode, bool deep)
{
	bool rt = true;
	rt &= serializeArrayElement( result, (papuga_Int)val.featidx(), errcode, deep);
	rt &= serializeArrayElement( result, val.weight(), errcode, deep);
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const SummaryElement& val, papuga_ErrorCode& errcode, bool deep)
{
	bool rt = true;
	rt &= serializeArrayElement( result, val.name(), errcode, deep);
	rt &= serializeArrayElement( result, val.value(), errcode, deep);
	rt &= serializeArrayElement( result, val.weight(), errcode, deep);
	rt &= serializeArrayElement( result, (papuga_Int)val.index(), errcode, deep);
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const analyzer::Document& val, papuga_ErrorCode& errcode, bool deep)
{
	bool rt = true;
	rt &= serializeArrayElement( result, val.subDocumentTypeName(), errcode, deep);
	rt &= serializeArrayElement( result, val.attributes(), errcode, deep);
	rt &= serializeArrayElement( result, val.metadata(), errcode, deep);
	rt &= serializeArrayElement( result, val.forwardIndexTerms(), errcode, deep);
	rt &= serializeArrayElement( result, val.searchIndexTerms(), errcode, deep);
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
bool Serializer::serialize_nothrow( papuga_Serialization* result, const TermExpression& val, papuga_ErrorCode& errcode, bool deep)
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
					papuga_Serialization_set_structid( ser, StructIdTemplate<analyzer::QueryTerm>::structid());
					stk.push_back( ser);
					const char* variablename = getTermExpressionVariableName( val, ii, ie);
					rt &= Serializer::serialize_nothrow( ser, expr.term( ii->idx()), variablename, errcode, deep);
					break;
				}
				case analyzer::QueryTermExpression::Instruction::Operator:
				{
					if (!val.isOperator( ii->idx())) return false;

					papuga_Serialization* ser = papuga_Allocator_alloc_Serialization( result->allocator);
					if (!ser) return false;
					papuga_Serialization_set_structid( ser, StructIdTemplate<analyzer::QueryTermExpression>::structid());

					const TermExpression::Operator& op = val.operatorStruct( ii->idx());
					const char* variablename = getTermExpressionVariableName( val, ii, ie);

					rt &= Serializer::serializeStructMemberConstName( ser, "op", op.name, errcode, deep);
					if (op.range) rt &= Serializer::serializeStructMemberConstName( ser, "range", (papuga_Int)op.range, errcode, deep);
					if (op.cardinality) rt &= Serializer::serializeStructMemberConstName( ser, "cardinality", (papuga_Int)op.cardinality, errcode, deep);
	
					rt &= papuga_Serialization_pushName_charp( ser, "arg");
					rt &= papuga_Serialization_pushOpen( ser);
					if ((std::size_t)ii->nofOperands() > stk.size()) throw std::runtime_error( _TXT("number of query analyzer expression operands out of range"));
					std::size_t si = stk.size() - ii->nofOperands(), se = stk.size();
					for (; si != se; ++si)
					{
						papuga_Serialization_pushValue_serialization( ser, stk[si]);
					}
					rt &= papuga_Serialization_pushClose( ser);//... end arg
					if (variablename)
					{
						rt &= Serializer::serializeStructMemberConstName( ser, "variable", variablename, errcode, deep);
					}
					stk.resize( stk.size() - ii->nofOperands());
					stk.push_back( ser);
					break;
				}
			}
		}
		if (val.singleUniqueResult())
		{
			// Copy serialization of unique result from the stack:
			if (stk.size() == 1)
			{
				papuga_Serialization* ser = *stk.begin();
				papuga_SerializationIter seriter;
				papuga_init_SerializationIter( &seriter, ser);
				for (; !papuga_SerializationIter_eof( &seriter); papuga_SerializationIter_skip( &seriter))
				{
					rt &= papuga_Serialization_push( result, papuga_SerializationIter_tag( &seriter), papuga_SerializationIter_value( &seriter));
				}
			}
			else if (stk.empty())
			{
				throw std::runtime_error( _TXT("no result returned by term expression analysis"));
			}
			else
			{
				throw std::runtime_error( _TXT("result returned by term expression analysis is not unique"));
			}
		}
		else
		{
			// Collect list of results from the stack:
			std::vector<papuga_Serialization*>::const_iterator si = stk.begin(), se = stk.end();
			for (; si != se; ++si)
			{
				rt &= papuga_Serialization_pushValue_serialization( result, *si);
			}
		}
		return rt;
	}
	catch (const std::bad_alloc&)
	{
		return false;
	}
}

bool Serializer::serialize_nothrow( papuga_Serialization* result, const MetaDataComparison& val, papuga_ErrorCode& errcode, bool deep)
{
	bool rt = true;
	papuga_ErrorCode err = papuga_Ok;
	papuga_ValueVariant buf;
	papuga_ValueVariant termval;
	papuga_init_ValueVariant_string( &termval, val.term()->value().c_str(), val.term()->value().size());
	papuga_ValueVariant* num = papuga_ValueVariant_tonumeric( &termval, &buf, &err);
	rt &= serializeArrayElement( result, val.cmpop(), errcode, deep);
	rt &= serializeArrayElement( result, val.term()->type(), errcode, deep);
	if (num)
	{
		rt &= serializeArrayElement( result, *num, errcode, deep);
	}
	else
	{
		rt &= serializeArrayElement( result, val.term()->value(), errcode, deep);
	}
	return rt;
}

bool Serializer::serialize_nothrow( papuga_Serialization* result, const MetaDataExpression& val, papuga_ErrorCode& errcode, bool deep)
{
	try
	{
		bool rt = true;
		const analyzer::QueryTermExpression& expr = val.expression();
		int termc = 0;

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
			std::vector<MetaDataComparison>::const_iterator cn = ci;
			unsigned int argcnt = 1;
			for (++cn; cn != ce && !cn->newGroup(); ++cn,++argcnt){}
			if (argcnt > 1)
			{
				rt &= papuga_Serialization_pushOpen( result);
				for (; ci != cn; ++ci)
				{
					rt &= serializeArrayElement( result, *ci, errcode, deep);
				}
				--ci;
				rt &= papuga_Serialization_pushClose( result);
			}
			else
			{
				rt &= serializeArrayElement( result, *ci, errcode, deep);
			}
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
bool Serializer::serialize_nothrow( papuga_Serialization* result, const TermStatisticsChange& val, papuga_ErrorCode& errcode, bool deep)
{
	bool rt = true;
	rt &= serializeArrayElement( result, val.type(), errcode, true/*deep=true always*/);
	rt &= serializeArrayElement( result, val.value(), errcode, true/*deep=true always*/);
	rt &= serializeArrayElement( result, (papuga_Int)val.increment(), errcode, deep);
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, StatisticsViewerInterface& val, papuga_ErrorCode& errcode, bool deep)
{
	bool rt = true;

	rt &= papuga_Serialization_pushOpen( result);

	TermStatisticsChange rec;
	while (val.nextDfChange( rec))
	{
		rt &= serializeArrayElement( result, rec, errcode, deep);
	}
	rt &= papuga_Serialization_pushClose( result);

	rt &= papuga_Serialization_pushValue_int( result, val.nofDocumentsInsertedChange());
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const ResultDocument& val, papuga_ErrorCode& errcode, bool deep)
{
	bool rt = true;
	rt &= serializeArrayElement( result, (papuga_Int)val.docno(), errcode, deep);
	rt &= serializeArrayElement( result, val.weight(), errcode, deep);
	rt &= serializeArrayElement( result, val.summaryElements(), errcode, deep);
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const QueryResult& val, papuga_ErrorCode& errcode, bool deep)
{
	bool rt = true;
	rt &= serializeArrayElement( result, (papuga_Int)val.evaluationPass(), errcode, deep);
	rt &= serializeArrayElement( result, (papuga_Int)val.nofRanked(), errcode, deep);
	rt &= serializeArrayElement( result, (papuga_Int)val.nofVisited(), errcode, deep);
	rt &= serializeArrayElement( result, val.ranks(), errcode, deep);
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const ConfigurationItemList& val, papuga_ErrorCode& errcode, bool deep)
{
	bool rt = true;
	ConfigurationItemList::const_iterator ci = val.begin(), ce = val.end();
	for (; ci != ce; ++ci)
	{
		const char* name = ci->first.c_str();
		std::size_t namelen = ci->first.size();
		const char* value = ci->second.c_str();
		std::size_t valuelen = ci->second.size();
		if (deep)
		{
			name = papuga_Allocator_copy_string( result->allocator, name, namelen);
			value = papuga_Allocator_copy_string( result->allocator, value, valuelen);
		}
		rt &= papuga_Serialization_pushName_string( result, name, namelen);
		if (name[0] == 'm' && 0==std::strcmp( name, "metadata"))
		{
			rt &= papuga_Serialization_pushOpen( result);
			strus::LocalErrorBuffer errbuf;
			ConfigurationItemList subitems = strus::getSubConfigStringItems( ci->second, &errbuf);
			if (errbuf.hasError())
			{
				errcode = (errbuf.errorCode() == ErrorCodeOutOfMem) ? papuga_NoMemError : papuga_SyntaxError;
				return false;
			}
			rt &= serialize_nothrow( result, subitems, errcode, true);
			rt &= papuga_Serialization_pushClose( result);
		}
		else
		{
			rt &= papuga_Serialization_pushValue_string( result, value, valuelen);
		}
	}
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<VectorQueryResult>& val, papuga_ErrorCode& errcode, bool deep)
{
	return serializeArray( result, val, errcode, deep);
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<std::string>& val, papuga_ErrorCode& errcode, bool deep)
{
	return serializeArray( result, val, errcode, deep);
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<int>& val, papuga_ErrorCode& errcode, bool deep)
{
	return serializeIntArray( result, val, errcode, deep);
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<double>& val, papuga_ErrorCode& errcode, bool deep)
{
	return serializeArray( result, val, errcode, deep);
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<analyzer::DocumentTerm>& val, papuga_ErrorCode& errcode, bool deep)
{
	return serializeArray( result, val, errcode, deep);
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<analyzer::DocumentMetaData>& val, papuga_ErrorCode& errcode, bool deep)
{
	bool rt = true;
	std::vector<analyzer::DocumentMetaData>::const_iterator vi = val.begin(), ve = val.end();
	for (; vi != ve; ++vi)
	{
		rt &= serializeStructMember( result, vi->name().c_str(), vi->value(), errcode, deep);
	}
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<analyzer::DocumentAttribute>& val, papuga_ErrorCode& errcode, bool deep)
{
	bool rt = true;
	std::vector<analyzer::DocumentAttribute>::const_iterator vi = val.begin(), ve = val.end();
	for (; vi != ve; ++vi)
	{
		rt &= serializeStructMember( result, vi->name().c_str(), vi->value(), errcode, deep);
	}
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const analyzer::ContentStatisticsItem& val, papuga_ErrorCode& errcode, bool deep)
{
	bool rt = true;
	rt &= serializeArrayElement( result, val.select(), errcode, deep);
	rt &= serializeArrayElement( result, val.type(), errcode, deep);
	rt &= serializeArrayElement( result, val.example(), errcode, deep);
	rt &= serializeArrayElement( result, (papuga_Int)val.df(), errcode, deep);
	rt &= serializeArrayElement( result, (papuga_Int)val.tf(), errcode, deep);
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<analyzer::ContentStatisticsItem>& val, papuga_ErrorCode& errcode, bool deep)
{
	return serializeArray( result, val, errcode, deep);
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const analyzer::ContentStatisticsResult& val, papuga_ErrorCode& errcode, bool deep)
{
	bool rt = true;
	rt &= serializeArrayElement( result, (papuga_Int)val.nofDocuments(), errcode, deep);
	rt &= serializeArrayElement( result, val.items(), errcode, deep);
	return rt;
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<strus::SummaryElement>& val, papuga_ErrorCode& errcode, bool deep)
{
	return serializeArray( result, val, errcode, deep);
}
bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<ResultDocument>& val, papuga_ErrorCode& errcode, bool deep)
{
	return serializeArray( result, val, errcode, deep);
}

bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<strus::FunctionDescription::Parameter>& val, papuga_ErrorCode& errcode, bool deep)
{
	return serializeArray( result, val, errcode, deep);
}

bool Serializer::serialize_nothrow( papuga_Serialization* result, const std::vector<strus::DebugTraceMessage>& val, papuga_ErrorCode& errcode, bool deep)
{
	return serializeArray( result, val, errcode, deep);
}

bool Serializer::serialize_nothrow( papuga_Serialization* result, const strus::FunctionDescription::Parameter& val, papuga_ErrorCode& errcode, bool deep)
{
	bool rt = true;
	rt &= Serializer::serializeStructMemberConstName( result, "type", val.typeName(), errcode, deep);
	rt &= Serializer::serializeStructMemberConstName( result, "name", val.name(), errcode, deep);
	rt &= Serializer::serializeStructMemberConstName( result, "domain", val.domain(), errcode, deep);
	rt &= Serializer::serializeStructMemberConstName( result, "text", val.text(), errcode, deep);
	return rt;
}

bool Serializer::serialize_nothrow( papuga_Serialization* result, const strus::FunctionDescription& val, papuga_ErrorCode& errcode, bool deep)
{
	bool rt = true;
	rt &= Serializer::serializeStructMemberConstName( result, "text", val.text(), errcode, deep);
	rt &= Serializer::serializeStructMemberConstName( result, "parameter", val.parameter(), errcode, deep);
	return rt;
}

bool Serializer::serialize_nothrow( papuga_Serialization* result, const strus::PostingJoinOperatorInterface::Description& val, papuga_ErrorCode& errcode, bool deep)
{
	bool rt = true;
	rt &= Serializer::serializeStructMemberConstName( result, "name", val.name(), errcode, deep);
	rt &= Serializer::serializeStructMemberConstName( result, "text", val.text(), errcode, deep);
	return rt;
}

bool Serializer::serialize_nothrow( papuga_Serialization* result, const DebugTraceMessage& val, papuga_ErrorCode& errcode, bool deep)
{
	bool rt = true;
	rt &= Serializer::serializeStructMemberConstName( result, "type", val.typeName(), errcode, deep);
	rt &= Serializer::serializeStructMemberConstName( result, "component", val.component(), errcode, deep);
	rt &= Serializer::serializeStructMemberConstName( result, "id", val.id(), errcode, deep);
	rt &= Serializer::serializeStructMemberConstName( result, "content", val.content(), errcode, deep);
	return rt;
}

