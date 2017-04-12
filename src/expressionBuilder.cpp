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

void PostProcPatternExpressionBuilder::pushExpression( const std::string& op, int argc, int range, unsigned int cardinality)
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

void PreProcPatternExpressionBuilder::pushExpression( const std::string& op, int argc, int range, unsigned int cardinality)
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

void QueryExpressionBuilder::pushTerm( const std::string& type, const std::string& value, int length)
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

void QueryExpressionBuilder::pushExpression( const std::string& opname, int argc, int range, unsigned int cardinality)
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

