/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_EXPRESSION_BUILDER_HPP_INCLUDED
#define _STRUS_BINDING_EXPRESSION_BUILDER_HPP_INCLUDED
#include "strus/queryInterface.hpp"
#include "strus/patternMatcherInstanceInterface.hpp"
#include "strus/patternTermFeederInstanceInterface.hpp"
#include "strus/patternLexerInstanceInterface.hpp"
#include "strus/queryProcessorInterface.hpp"
#include "strus/queryAnalyzerInstanceInterface.hpp"
#include "strus/queryAnalyzerContextInterface.hpp"
#include "strus/postingIteratorInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/reference.hpp"
#include "strus/base/symbolTable.hpp"
#include "queryAnalyzerStruct.hpp"
#include "impl/value/termExpression.hpp"
#include <string>
#include <vector>

namespace strus {
namespace bindings {

class ExpressionBuilder
{
public:
	virtual ~ExpressionBuilder(){}
	virtual void pushTerm( const std::string& type, const std::string& value, unsigned int length)=0;
	virtual void pushTerm( const std::string& type, const std::string& value)=0;
	virtual void pushTerm( const std::string& type)=0;
	virtual void pushDocField( const std::string& metadataRangeStart, const std::string& metadataRangeEnd)=0;
	virtual void pushExpression( const std::string& op, unsigned int argc, int range, unsigned int cardinality)=0;
	virtual void attachVariable( const std::string& name)=0;
	virtual void definePattern( const std::string& name, const std::string& formatstring, bool visible)=0;
};

class PostProcPatternExpressionBuilder
	:public ExpressionBuilder
{
public:
	PostProcPatternExpressionBuilder( PatternMatcherInstanceInterface* matcher_, PatternTermFeederInstanceInterface* feeder_, ErrorBufferInterface* errorhnd_)
		:m_errorhnd(errorhnd_),m_matcher(matcher_),m_feeder(feeder_){}

	virtual ~PostProcPatternExpressionBuilder(){}
	virtual void pushTerm( const std::string& type, const std::string& value, unsigned int length);
	virtual void pushTerm( const std::string& type, const std::string& value);
	virtual void pushTerm( const std::string& type);
	virtual void pushDocField( const std::string& metadataRangeStart, const std::string& metadataRangeEnd);
	virtual void pushExpression( const std::string& op, unsigned int argc, int range, unsigned int cardinality);
	virtual void attachVariable( const std::string& name);
	virtual void definePattern( const std::string& name, const std::string& formatstring, bool visible);

	void defineLexem( const std::string& name);
	
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
	virtual void pushTerm( const std::string& type, const std::string& value, unsigned int length);
	virtual void pushTerm( const std::string& type, const std::string& value);
	virtual void pushTerm( const std::string& type);
	virtual void pushDocField( const std::string& metadataRangeStart, const std::string& metadataRangeEnd);
	virtual void pushExpression( const std::string& op, unsigned int argc, int range, unsigned int cardinality);
	virtual void attachVariable( const std::string& name);
	virtual void definePattern( const std::string& name, const std::string& formatstring, bool visible);

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
		:m_errorhnd(errorhnd_),m_queryproc(queryproc_),m_query(query_),m_stackSize(0){}
	virtual ~QueryExpressionBuilder(){}
	virtual void pushTerm( const std::string& type, const std::string& value, unsigned int length);
	virtual void pushTerm( const std::string& type, const std::string& value);
	virtual void pushTerm( const std::string& type);
	virtual void pushDocField( const std::string& metadataRangeStart, const std::string& metadataRangeEnd);
	virtual void pushExpression( const std::string& op, unsigned int argc, int range, unsigned int cardinality);
	virtual void attachVariable( const std::string& name);
	virtual void definePattern( const std::string& name, const std::string& formatstring, bool visible);

	unsigned int stackSize() const					{return m_stackSize > 0 ? m_stackSize:0;}

private:
	QueryExpressionBuilder( const QueryExpressionBuilder&){}	//< non copyable
	void operator=( const QueryExpressionBuilder&){}		//< non copyable

private:
	ErrorBufferInterface* m_errorhnd;
	const QueryProcessorInterface* m_queryproc;
	QueryInterface* m_query;
	int m_stackSize;
};

class QueryAnalyzerTermExpressionBuilder
	:public ExpressionBuilder
{
public:
	explicit QueryAnalyzerTermExpressionBuilder( TermExpression* expression_)
		:m_expression(expression_){}
	virtual ~QueryAnalyzerTermExpressionBuilder(){}

	virtual void pushTerm( const std::string& fieldtype, const std::string& value, unsigned int length);
	virtual void pushTerm( const std::string& fieldtype, const std::string& value);
	virtual void pushTerm( const std::string& value);
	virtual void pushDocField( const std::string& metadataRangeStart, const std::string& metadataRangeEnd);
	virtual void pushExpression( const std::string& op, unsigned int argc, int range, unsigned int cardinality);
	virtual void attachVariable( const std::string& name);
	virtual void definePattern( const std::string& name, const std::string& formatstring, bool visible);

private:
	QueryAnalyzerTermExpressionBuilder( const QueryAnalyzerTermExpressionBuilder&){}	//< non copyable
	void operator=( const QueryAnalyzerTermExpressionBuilder&){}				//< non copyable

private:
	void pushField( const std::string& fieldtype, const std::string& value);

	TermExpression* m_expression;
};

class PostingsExpressionBuilder
	:public ExpressionBuilder
{
public:
	PostingsExpressionBuilder( const StorageClientInterface* storage_, const QueryProcessorInterface* queryproc_, ErrorBufferInterface* errorhnd_)
		:m_errorhnd(errorhnd_),m_storage(storage_),m_queryproc(queryproc_){}

	virtual ~PostingsExpressionBuilder(){}
	virtual void pushTerm( const std::string& type, const std::string& value, unsigned int length);
	virtual void pushTerm( const std::string& type, const std::string& value);
	virtual void pushTerm( const std::string& type);
	virtual void pushDocField( const std::string& metadataRangeStart, const std::string& metadataRangeEnd);
	virtual void pushExpression( const std::string& op, unsigned int argc, int range, unsigned int cardinality);
	virtual void attachVariable( const std::string& name);
	virtual void definePattern( const std::string& name, const std::string& formatstring, bool visible);

public:
	Reference<PostingIteratorInterface> pop();

private:
	PostingsExpressionBuilder( const PostingsExpressionBuilder&){}	//< non copyable
	void operator=( const QueryExpressionBuilder&){}		//< non copyable

private:
	ErrorBufferInterface* m_errorhnd;
	const StorageClientInterface* m_storage;
	const QueryProcessorInterface* m_queryproc;
	std::vector<Reference<PostingIteratorInterface> > m_stack;
};

}}//namespace
#endif
