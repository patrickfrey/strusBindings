/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "expressionBuilder.hpp"
#include "internationalization.hpp"
#include "strus/base/stdint.h"
#include "strus/base/utf8.hpp"
#include "strus/postingJoinOperatorInterface.hpp"
#include <stdexcept>
/*[-]*/#include <cstdio>

using namespace strus;
using namespace strus::bindings;

enum {MaxPatternTermNameId=(1<<24)};

static strus::PatternMatcherInstanceInterface::JoinOperation
	patternMatcherJoinOp( const std::string& opname)
{
	if (opname == "sequence")
	{
		return strus::PatternMatcherInstanceInterface::OpSequence;
	}
	else if (opname == "sequence_imm")
	{
		return strus::PatternMatcherInstanceInterface::OpSequenceImm;
	}
	else if (opname == "sequence_struct")
	{
		return strus::PatternMatcherInstanceInterface::OpSequenceStruct;
	}
	else if (opname == "within")
	{
		return strus::PatternMatcherInstanceInterface::OpWithin;
	}
	else if (opname == "within_struct")
	{
		return strus::PatternMatcherInstanceInterface::OpWithinStruct;
	}
	else if (opname == "any")
	{
		return strus::PatternMatcherInstanceInterface::OpAny;
	}
	else if (opname == "and")
	{
		return strus::PatternMatcherInstanceInterface::OpAnd;
	}
	else
	{
		throw strus::runtime_error(_TXT("unknown operator '%s' in pattern expression"), opname.c_str());
	}
}

static std::string termSymbolKey( unsigned int termid, const std::string& name)
{
	char termidbuf[ 16];
	std::size_t termidsize = strus::utf8encode( termidbuf, termid+1);
	std::string symkey( termidbuf, termidsize);
	symkey.append( name);
	return symkey;
}

void PostProcPatternExpressionBuilder::pushTerm( const std::string& type, const std::string& value, unsigned int length)
{
	throw strus::runtime_error(_TXT("length parameter not implemented for pattern input terms"));
}

void PostProcPatternExpressionBuilder::pushTerm( const std::string& type, const std::string& value)
{
	uint32_t termtypeid = m_termtypetab.get( type);
	if (!termtypeid)
	{
		throw strus::runtime_error(_TXT("lexem with type '%s' is not defined"), type.c_str());
	}
	uint32_t termsymid = m_termsymtab.getOrCreate( termSymbolKey( termtypeid, value));
	if (!termsymid) throw std::bad_alloc();
	termsymid += MaxPatternTermNameId;
	if (m_termsymtab.isNew())
	{
		m_feeder->defineSymbol( termsymid, termtypeid, value);
	}
	m_matcher->pushTerm( termsymid);
}

void PostProcPatternExpressionBuilder::pushTerm( const std::string& type)
{
	uint32_t termtypeid = m_termtypetab.get( type);
	if (!termtypeid)
	{
		m_matcher->pushPattern( type);
	}
	else
	{
		m_matcher->pushTerm( termtypeid);
	}
}

void PostProcPatternExpressionBuilder::pushDocField( const std::string& metadataRangeStart, const std::string& metadataRangeEnd)
{
	throw strus::runtime_error(_TXT("document field ranges ([%s..%s]) not implemented in pattern expressions"), metadataRangeStart.c_str(), metadataRangeEnd.c_str());
}

void PostProcPatternExpressionBuilder::pushExpression( const std::string& op, unsigned int argc, int range, unsigned int cardinality)
{
	strus::PatternMatcherInstanceInterface::JoinOperation joinop = patternMatcherJoinOp( op);
	m_matcher->pushExpression( joinop, argc, range, cardinality);
}

void PostProcPatternExpressionBuilder::attachVariable( const std::string& name)
{
	m_matcher->attachVariable( name);
}

void PostProcPatternExpressionBuilder::definePattern( const std::string& name, bool visible)
{
	m_matcher->definePattern( name, visible);
}

void PostProcPatternExpressionBuilder::defineLexem( const std::string& name)
{
	uint32_t termtypeid = m_termtypetab.getOrCreate( name);
	if (!termtypeid) throw std::bad_alloc();
	if (m_termtypetab.isNew())
	{
		if (termtypeid >= MaxPatternTermNameId) throw strus::runtime_error(_TXT("too many lexems defined in pattern match program"));
		m_feeder->defineLexem( termtypeid, name);
	}
	else
	{
		throw strus::runtime_error(_TXT("duplicate definition of lexem '%s'"), name.c_str());
	}
}

void PreProcPatternExpressionBuilder::pushTerm( const std::string& type, const std::string& value, unsigned int length)
{
	throw strus::runtime_error(_TXT("length parameter not implemented for pattern input terms"));
}

void PreProcPatternExpressionBuilder::pushTerm( const std::string& type, const std::string& value)
{
	uint32_t termtypeid = m_termtypetab.get( type);
	if (!termtypeid)
	{
		throw strus::runtime_error(_TXT("lexem with type '%s' is not defined"), type.c_str());
	}
	uint32_t termsymid = m_termsymtab.getOrCreate( termSymbolKey( termtypeid, value));
	if (!termsymid) throw std::bad_alloc();
	termsymid += MaxPatternTermNameId;
	if (m_termsymtab.isNew())
	{
		m_lexer->defineSymbol( termsymid, termtypeid, value);
	}
	m_matcher->pushTerm( termsymid);
}

void PreProcPatternExpressionBuilder::pushTerm( const std::string& type)
{
	uint32_t termtypeid = m_termtypetab.get( type);
	if (!termtypeid)
	{
		m_matcher->pushPattern( type);
	}
	else
	{
		m_matcher->pushTerm( termtypeid);
	}
}

void PreProcPatternExpressionBuilder::pushDocField( const std::string& metadataRangeStart, const std::string& metadataRangeEnd)
{
	throw strus::runtime_error(_TXT("document field ranges ([%s..%s]) not implemented in pattern expressions"), metadataRangeStart.c_str(), metadataRangeEnd.c_str());
}

void PreProcPatternExpressionBuilder::pushExpression( const std::string& op, unsigned int argc, int range, unsigned int cardinality)
{
	strus::PatternMatcherInstanceInterface::JoinOperation joinop = patternMatcherJoinOp( op);
	m_matcher->pushExpression( joinop, argc, range, cardinality);
}

void PreProcPatternExpressionBuilder::attachVariable( const std::string& name)
{
	m_matcher->attachVariable( name);
}

void PreProcPatternExpressionBuilder::defineLexem(
		const std::string& name,
		const std::string& expression,
		unsigned int resultIndex,
		unsigned int level,
		analyzer::PositionBind posbind)
{
	uint32_t termtypeid = m_termtypetab.getOrCreate( name);
	if (!termtypeid) throw std::bad_alloc();
	m_lexer->defineLexem( termtypeid, expression, resultIndex, level, posbind);
}

void PreProcPatternExpressionBuilder::definePattern( const std::string& name, bool visible)
{
	m_matcher->definePattern( name, visible);
}

void QueryExpressionBuilder::pushTerm( const std::string& type, const std::string& value, unsigned int length)
{
	m_query->pushTerm( type, value, length);
}

void QueryExpressionBuilder::pushTerm( const std::string& type, const std::string& value)
{
	m_query->pushTerm( type, value, 1);
}

void QueryExpressionBuilder::pushTerm( const std::string& type)
{
	m_query->pushTerm( type, std::string(), 1);
}

void QueryExpressionBuilder::pushDocField( const std::string& metadataRangeStart, const std::string& metadataRangeEnd)
{
	m_query->pushDocField( metadataRangeStart, metadataRangeEnd);
}

void QueryExpressionBuilder::pushExpression( const std::string& opname, unsigned int argc, int range, unsigned int cardinality)
{
	const PostingJoinOperatorInterface* op = m_queryproc->getPostingJoinOperator( opname);
	if (!op) throw strus::runtime_error(_TXT("error getting posting join operator '%s': %s"), opname.c_str(), m_errorhnd->fetchError());

	m_query->pushExpression( op, argc, range, cardinality);
}

void QueryExpressionBuilder::attachVariable( const std::string& name)
{
	m_query->attachVariable( name);
}

void QueryExpressionBuilder::definePattern( const std::string& name, bool visible)
{
	throw strus::runtime_error(_TXT("define pattern not implemented for query"));
}

void QueryAnalyzerExpressionBuilder::pushTerm( const std::string& type, const std::string& value, unsigned int length)
{
	throw strus::runtime_error(_TXT("length parameter not allowed for query fields passed to analyzer"));
}

void QueryAnalyzerExpressionBuilder::pushField( const std::string& fieldtype, const std::string& value)
{
	/*[-]*/fprintf( stderr, "QueryAnalyzerExpressionBuilder::pushField %s '%s'\n", fieldtype.c_str(), value.c_str());
	typedef QueryAnalyzerStruct::GroupOperatorList GroupOperatorList;

	++m_fieldno_cnt;
	m_analyzer->putField( m_fieldno_cnt, fieldtype, value);
	m_fieldno_stack.push_back( m_fieldno_cnt);
	std::vector<unsigned int> fieldnoList( m_fieldno_stack.end()-1, m_fieldno_stack.end());

	const GroupOperatorList& gop = m_analyzerStruct->autoGroupOperators( fieldtype);
	GroupOperatorList::const_iterator gi = gop.begin(), ge = gop.end();
	for (; gi != ge; ++gi)
	{
		unsigned int groupid = m_operators.size();
		m_operators.push_back( gi->opr);
		m_analyzer->groupElements( groupid, fieldnoList, gi->groupBy, gi->groupSingle);
	}
}

void QueryAnalyzerExpressionBuilder::pushTerm( const std::string& type, const std::string& value)
{
	pushField( type, value);
}

void QueryAnalyzerExpressionBuilder::pushTerm( const std::string& value)
{
	pushField( std::string(), value);
}

void QueryAnalyzerExpressionBuilder::pushDocField( const std::string& metadataRangeStart, const std::string& metadataRangeEnd)
{
	throw strus::runtime_error(_TXT("document meta data ranges not implemented for query"));
}

void QueryAnalyzerExpressionBuilder::pushExpression( const std::string& op, unsigned int argc, int range, unsigned int cardinality)
{
	if (m_fieldno_stack.size() < argc) throw strus::runtime_error(_TXT("push expression without all arguments defined"));
	unsigned int* fnstart = m_fieldno_stack.data() + m_fieldno_stack.size() - argc;
	unsigned int* fnend = fnstart + argc;
	std::vector<unsigned int> fieldnoList( fnstart, fnend);

	unsigned int groupid = m_operators.size();
	m_operators.push_back( QueryAnalyzerStruct::Operator(
			QueryAnalyzerStruct::Operator::Expression,
			op, argc, range, cardinality));
	QueryAnalyzerContextInterface::GroupBy groupBy = QueryAnalyzerContextInterface::GroupAll;
	m_analyzer->groupElements( groupid, fieldnoList, groupBy, true/*groupSingle*/);
	m_fieldno_stack.resize( m_fieldno_stack.size() - argc + 1);
}

void QueryAnalyzerExpressionBuilder::attachVariable( const std::string& name)
{
	if (m_fieldno_stack.empty()) throw strus::runtime_error(_TXT("attach variable not allowed without query fields defined"));
	std::vector<unsigned int> fieldnoList( m_fieldno_stack.end()-1, m_fieldno_stack.end());

	unsigned int groupid = m_operators.size();
	m_operators.push_back( QueryAnalyzerStruct::Operator( 
			QueryAnalyzerStruct::Operator::Variable,
			name, 1, 0/*range*/, 0/*cardinality*/));
	QueryAnalyzerContextInterface::GroupBy groupBy = QueryAnalyzerContextInterface::GroupEvery;
	m_analyzer->groupElements( groupid, fieldnoList, groupBy, true/*groupSingle*/);
}

void QueryAnalyzerExpressionBuilder::definePattern( const std::string& name, bool visible)
{
	throw strus::runtime_error(_TXT("define pattern not implemented for query"));
}

