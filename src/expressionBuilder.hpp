/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_EXPRESSION_BUILDER_HPP_INCLUDED
#define _STRUS_BINDING_EXPRESSION_BUILDER_HPP_INCLUDED
#include "strus/bindings/valueVariant.hpp"
#include "strus/bindings/serialization.hpp"
#include "strus/queryInterface.hpp"
#include "strus/patternMatcherInstanceInterface.hpp"
#include "strus/patternTermFeederInstanceInterface.hpp"
#include "strus/patternLexerInstanceInterface.hpp"
#include "strus/queryProcessorInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/reference.hpp"
#include "strus/base/symbolTable.hpp"
#include <string>

namespace strus {
namespace bindings {

class ExpressionBuilder
{
public:
	virtual ~ExpressionBuilder();
	virtual void pushTerm( const std::string& type, const std::string& value, int length)=0;
	virtual void pushTerm( const std::string& type, const std::string& value)=0;
	virtual void pushDocField( const std::string& metadataRangeStart, const std::string& metadataRangeEnd)=0;
	virtual void pushReference( const std::string& name)=0;
	virtual void pushExpression( const std::string& op, int argc, int range, unsigned int cardinality)=0;
	virtual void defineItem( const std::string& name)=0;
	virtual void defineFeature( const std::string& name, double weight)=0;
	virtual void definePattern( const std::string& name, bool visible)=0;
	virtual void attachVariable( const std::string& name, double weight)=0;
	virtual void attachVariable( const std::string& name)=0;
};

class PostProcPatternExpressionBuilder
	:public ExpressionBuilder
{
public:
	PostProcPatternExpressionBuilder( PatternMatcherInstanceInterface* matcher_, PatternTermFeederInstanceInterface* feeder_, ErrorBufferInterface* errorhnd_)
		:m_errorhnd(errorhnd_),m_matcher(matcher_),m_feeder(feeder_){}

	virtual ~PostProcPatternExpressionBuilder(){}
	virtual void pushTerm( const std::string& type, const std::string& value, int length);
	virtual void pushTerm( const std::string& type, const std::string& value);
	virtual void pushDocField( const std::string& metadataRangeStart, const std::string& metadataRangeEnd);
	virtual void pushReference( const std::string& name);
	virtual void pushExpression( const std::string& op, int argc, int range, unsigned int cardinality);
	virtual void defineItem( const std::string& name);
	virtual void defineFeature( const std::string& name, double weight);
	virtual void definePattern( const std::string& name, bool visible);
	virtual void attachVariable( const std::string& name, double weight);
	virtual void attachVariable( const std::string& name);

private:
	PostProcPatternExpressionBuilder( const PostProcPatternExpressionBuilder&){}	//< non copyable
	void operator=( const PostProcPatternExpressionBuilder&){}			//< non copyable

private:
	ErrorBufferInterface* m_errorhnd;
	PatternMatcherInstanceInterface* m_matcher;
	PatternTermFeederInstanceInterface* m_feeder;
	strus::SymbolTable m_termsymtab;
	strus::SymbolTable m_termtypetab;
};

class PreProcPatternExpressionBuilder
	:public ExpressionBuilder
{
public:
	PreProcPatternExpressionBuilder( PatternMatcherInstanceInterface* matcher_, PatternLexerInstanceInterface* lexer_, ErrorBufferInterface* errorhnd_)
		:m_errorhnd(errorhnd_),m_matcher(matcher_),m_lexer(lexer_){}

	virtual ~PreProcPatternExpressionBuilder(){}
	virtual void pushTerm( const std::string& type, const std::string& value, int length);
	virtual void pushTerm( const std::string& type, const std::string& value);
	virtual void pushDocField( const std::string& metadataRangeStart, const std::string& metadataRangeEnd);
	virtual void pushReference( const std::string& name);
	virtual void pushExpression( const std::string& op, int argc, int range, unsigned int cardinality);
	virtual void defineItem( const std::string& name);
	virtual void defineFeature( const std::string& name, double weight);
	virtual void definePattern( const std::string& name, bool visible);
	virtual void attachVariable( const std::string& name, double weight);
	virtual void attachVariable( const std::string& name);

	void defineLexem(
			const std::string& name,
			const std::string& expression,
			unsigned int resultIndex,
			unsigned int level,
			analyzer::PositionBind posbind);

private:
	PreProcPatternExpressionBuilder( const PreProcPatternExpressionBuilder&){}	//< non copyable
	void operator=( const PreProcPatternExpressionBuilder&){}			//< non copyable

private:
	ErrorBufferInterface* m_errorhnd;
	PatternMatcherInstanceInterface* m_matcher;
	PatternLexerInstanceInterface* m_lexer;
	strus::SymbolTable m_termsymtab;
	strus::SymbolTable m_termtypetab;
};

class QueryExpressionBuilder
	:public ExpressionBuilder
{
public:
	QueryExpressionBuilder( QueryInterface* query_, const QueryProcessorInterface* queryproc_, ErrorBufferInterface* errorhnd_)
		:m_errorhnd(errorhnd_),m_queryproc(queryproc_),m_query(query_){}
	virtual ~QueryExpressionBuilder(){}
	virtual void pushTerm( const std::string& type, const std::string& value, int length);
	virtual void pushTerm( const std::string& type, const std::string& value);
	virtual void pushDocField( const std::string& metadataRangeStart, const std::string& metadataRangeEnd);
	virtual void pushReference( const std::string& name);
	virtual void pushExpression( const std::string& op, int argc, int range, unsigned int cardinality);
	virtual void defineItem( const std::string& name);
	virtual void defineFeature( const std::string& name, double weight);
	virtual void definePattern( const std::string& name, bool visible);
	virtual void attachVariable( const std::string& name, double weight);
	virtual void attachVariable( const std::string& name);

private:
	QueryExpressionBuilder( const QueryExpressionBuilder&){}	//< non copyable
	void operator=( const QueryExpressionBuilder&){}		//< non copyable

private:
	ErrorBufferInterface* m_errorhnd;
	const QueryProcessorInterface* m_queryproc;
	QueryInterface* m_query;
};

}}//namespace
#endif
