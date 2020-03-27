/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "expressionBuilder.hpp"
#include "private/internationalization.hpp"
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
	throw strus::runtime_error(_TXT("%s not implemented for %s %s"), "length parameter", "postproc pattern", "push term");
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

void PostProcPatternExpressionBuilder::pushExpression( const std::string& op, unsigned int argc, int range, unsigned int cardinality)
{
	strus::PatternMatcherInstanceInterface::JoinOperation joinop = patternMatcherJoinOp( op);
	m_matcher->pushExpression( joinop, argc, range, cardinality);
}

void PostProcPatternExpressionBuilder::attachVariable( const std::string& name)
{
	m_matcher->attachVariable( name);
}

void PostProcPatternExpressionBuilder::definePattern( const std::string& name, const std::string& formatstring, bool visible)
{
	m_matcher->definePattern( name, formatstring, visible);
}

void PostProcPatternExpressionBuilder::defineFeature( const std::string& featureSet, double weight)
{
	throw strus::runtime_error(_TXT("%s not implemented for %s"), "define feature","post proc pattern");
}

void PostProcPatternExpressionBuilder::defineLexem( const std::string& name)
{
	uint32_t termtypeid = m_termtypetab.getOrCreate( name);
	if (!termtypeid) throw std::bad_alloc();
	if (m_termtypetab.isNew())
	{
		if (termtypeid >= MaxPatternTermNameId) throw std::runtime_error( _TXT("too many lexems defined in pattern match program"));
		m_feeder->defineLexem( termtypeid, name);
	}
	else
	{
		throw strus::runtime_error(_TXT("duplicate definition of lexem '%s'"), name.c_str());
	}
}

void PreProcPatternExpressionBuilder::pushTerm( const std::string& type, const std::string& value, unsigned int length)
{
	throw strus::runtime_error(_TXT("%s not implemented for %s"), "length parameter","pre proc pattern");
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
		m_lexer->defineLexemName( termsymid, value);
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
	if (m_termtypetab.isNew())
	{
		m_lexer->defineLexemName( termtypeid, name);
	}
	m_lexer->defineLexem( termtypeid, expression, resultIndex, level, posbind);
}

void PreProcPatternExpressionBuilder::definePattern( const std::string& name, const std::string& formatstring, bool visible)
{
	m_matcher->definePattern( name, formatstring, visible);
}

void PreProcPatternExpressionBuilder::defineFeature( const std::string& featureSet, double weight)
{
	throw strus::runtime_error(_TXT("%s not implemented for %s"), "define feature","pre proc pattern");
}

void QueryExpressionBuilder::pushTerm( const std::string& type, const std::string& value, unsigned int length)
{
	++m_stackSize;
	m_query->pushTerm( type, value, length);
}

void QueryExpressionBuilder::pushTerm( const std::string& type, const std::string& value)
{
	++m_stackSize;
	m_query->pushTerm( type, value, 1);
}

void QueryExpressionBuilder::pushTerm( const std::string& type)
{
	++m_stackSize;
	throw strus::runtime_error(_TXT("push term without value not implemented for %s"), "query");
}

void QueryExpressionBuilder::pushExpression( const std::string& opname, unsigned int argc, int range, unsigned int cardinality)
{
	const PostingJoinOperatorInterface* op = m_queryproc->getPostingJoinOperator( opname);
	if (!op) throw strus::runtime_error(_TXT("error getting posting join operator '%s': %s"), opname.c_str(), m_errorhnd->fetchError());

	m_stackSize = m_stackSize - argc + 1;
	m_query->pushExpression( op, argc, range, cardinality);
}

void QueryExpressionBuilder::attachVariable( const std::string& name)
{
	m_query->attachVariable( name);
}

void QueryExpressionBuilder::definePattern( const std::string& name, const std::string& formatstring, bool visible)
{
	throw strus::runtime_error(_TXT("%s not implemented for %s"), "define pattern", "query expression");
}

void QueryExpressionBuilder::defineFeature( const std::string& featureSet, double weight)
{
	throw strus::runtime_error(_TXT("%s not implemented for %s"), "define feature","query expression");
}

void QueryAnalyzerTermExpressionBuilder::pushTerm( const std::string& type, const std::string& value, unsigned int length)
{
	throw std::runtime_error( _TXT("length parameter not allowed for query fields passed to analyzer"));
}

void QueryAnalyzerTermExpressionBuilder::pushField( const std::string& fieldtype, const std::string& value)
{
	m_expression->pushField( fieldtype, value);
}

void QueryAnalyzerTermExpressionBuilder::pushTerm( const std::string& type, const std::string& value)
{
	pushField( type, value);
}

void QueryAnalyzerTermExpressionBuilder::pushTerm( const std::string& value)
{
	pushField( std::string(), value);
}

void QueryAnalyzerTermExpressionBuilder::pushExpression( const std::string& op, unsigned int argc, int range, unsigned int cardinality)
{
	m_expression->pushExpression( op, argc, range, cardinality);
}

void QueryAnalyzerTermExpressionBuilder::attachVariable( const std::string& name)
{
	m_expression->attachVariable( name);
}

void QueryAnalyzerTermExpressionBuilder::definePattern( const std::string& name, const std::string& formatstring, bool visible)
{
	throw strus::runtime_error(_TXT("%s not implemented for %s"), "define pattern", "query analyzer term expression");
}

void QueryAnalyzerTermExpressionBuilder::defineFeature( const std::string& featureSet, double weight)
{
	throw strus::runtime_error(_TXT("%s not implemented for %s"), "define feature","query analyzer term expression");
}

void PostingsExpressionBuilder::pushTerm( const std::string& type, const std::string& value, unsigned int length)
{
	Reference<PostingIteratorInterface> itr( m_storage->createTermPostingIterator( type, value, length, getTermStatistics(type,value)));
	if (!itr.get()) throw strus::runtime_error(_TXT("failed to create term posting iterator for %s '%s': %s"), type.c_str(), value.c_str(), m_errorhnd->fetchError());
	m_stack.push_back( itr);
}

void PostingsExpressionBuilder::pushTerm( const std::string& type, const std::string& value)
{
	Reference<PostingIteratorInterface> itr( m_storage->createTermPostingIterator( type, value, 1, getTermStatistics(type,value)));
	if (!itr.get()) throw strus::runtime_error(_TXT("failed to create term posting iterator for %s '%s': %s"), type.c_str(), value.c_str(), m_errorhnd->fetchError());
	m_stack.push_back( itr);
}

void PostingsExpressionBuilder::pushTerm( const std::string& type)
{
	throw strus::runtime_error(_TXT("push term without value not implemented for %s"), "posting iterator");
}

void PostingsExpressionBuilder::pushExpression( const std::string& op, unsigned int argc, int range, unsigned int cardinality)
{
	if (argc > m_stack.size())
	{
		throw std::runtime_error( _TXT("too few arguments on stack for operation"));
	}
	else
	{
		const PostingJoinOperatorInterface* joinop = m_queryproc->getPostingJoinOperator( op);
		if (!joinop)
		{
			throw strus::runtime_error(_TXT("posting join operator '%s' not defined"), op.c_str());
		}
		std::vector<Reference<PostingIteratorInterface> >::const_iterator
			si = m_stack.end() - argc, se = m_stack.end();
		std::vector<Reference<PostingIteratorInterface> > args;
		args.insert( args.end(), si, se);
		Reference<PostingIteratorInterface> itr( joinop->createResultIterator( args, range, cardinality));
		if (!itr.get()) throw strus::runtime_error(_TXT("failed to create posting iterator join for '%s': %s"), op.c_str(), m_errorhnd->fetchError());
		m_stack.resize( m_stack.size() - argc);
		m_stack.push_back( itr);
	}
}

void PostingsExpressionBuilder::attachVariable( const std::string& name)
{
	throw strus::runtime_error(_TXT("%s is not implemented for %s"), "attach variable", "postings iterator");
}

void PostingsExpressionBuilder::definePattern( const std::string& name, const std::string& formatstring, bool visible)
{
	throw strus::runtime_error(_TXT("%s is not implemented for %s"), "define pattern", "postings iterator");
}

void PostingsExpressionBuilder::defineFeature( const std::string& featureSet, double weight)
{
	throw strus::runtime_error(_TXT("%s not implemented for %s"), "define feature","postings iterator");
}

Reference<PostingIteratorInterface> PostingsExpressionBuilder::pop()
{
	if (m_stack.empty()) throw std::runtime_error( _TXT("no posting expression on stack as result of parsing expression"));
	Reference<PostingIteratorInterface> rt = m_stack.back();
	m_stack.pop_back();
	if (!m_stack.empty()) throw std::runtime_error( _TXT("more than one posting expression on stack as result of parsing expression"));
	return rt;
}

