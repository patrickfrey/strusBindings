/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* \brief Schema for query analysis and evaluation
 * @file schemas_query.hpp
 */
#ifndef _STRUS_WEBREQUEST_SCHEMAS_QUERY_HPP_INCLUDED
#define _STRUS_WEBREQUEST_SCHEMAS_QUERY_HPP_INCLUDED
#include "schemas_base.hpp"
#include "schemas_expression.hpp"
#include "schemas_analyzer.hpp"
#include "schemas_query_decl.hpp"
#include "schemas_queryeval_decl.hpp"

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class SchemaQueryPart :public AutomatonNameSpace
{
public:
	static papuga::RequestAutomaton_NodeList buildQueryOriginalAnalyzed( const char* rootexpr)
	{
		typedef bindings::method::Query Q;
		return { rootexpr, {
			{SchemaAnalyzerPart::analyzeQuery("")},
			{"feature", 0, "query", Q::addFeature(), {{FeatureSet}, {"_analyzed"}, {FeatureWeight, '?'}} },
			{"restriction", 0, "query", Q::addMetaDataRestriction(),  {"_analyzed"} }
		}};
	}

	static papuga::RequestAutomaton_NodeList buildQueryAnalyzed( const char* rootexpr)
	{
		typedef bindings::method::Query Q;
		return { rootexpr, {
			{SchemaQueryDeclPart::declareFeature( "")},
			{"feature", 0, "query", Q::addFeature(), {{FeatureSet}, {TermExpression, '!', 2/*tag diff*/}, {FeatureWeight, '?'}} },

			{SchemaQueryDeclPart::declareMetaData( "")},
			{"restriction", 0, "query", Q::addMetaDataRestriction(),  {MetaDataCondition} }
		}};
	}

	static papuga::RequestAutomaton_NodeList createQuery( const char* rootexpr)
	{
		typedef bindings::method::QueryEval QE;
		return { rootexpr, {
			{SchemaQueryEvalDeclPart::defineQueryEval( "eval")},
			{SchemaAnalyzerPart::defineQueryAnalyzer( "analyzer")},
			{"", "query", "qryeval", QE::createQuery(), {{"storage"}} }
		}};
	}

	static papuga::RequestAutomaton_NodeList evaluateQuery( const char* rootexpr)
	{
		typedef bindings::method::Query Q;
		return { rootexpr, {
			{SchemaQueryDeclPart::defineStatistics( "")},
			{SchemaQueryDeclPart::defineRankingParameter( "")},
			{"", "ranklist", "query", Q::evaluate(), {} }
		}};
	}
};

class Schema_Context_INIT_QueryEval :public papuga::RequestAutomaton
{
public:
	Schema_Context_INIT_QueryEval() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		{},
		{
			{"storage","/qryeval/include/storage()",false/*not required*/},
			{"qryanalyzer","/qryeval/include/analyzer()",false/*not required*/}
		},
		{
			{SchemaQueryEvalDeclPart::defineQueryEval( "/qryeval")}
		}
	) {}
};

class Schema_Context_PUT_QueryEval :public Schema_Context_INIT_QueryEval
{
public:
	Schema_Context_PUT_QueryEval() :Schema_Context_INIT_QueryEval(){}
};

class Schema_QueryEval_GET :public papuga::RequestAutomaton
{
public:
	Schema_QueryEval_GET() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		{
			{"result", { {"/query", "ranklist", "ranklist", '!'} }}
		},
		{},
		{
			{SchemaQueryPart::createQuery( "/query")},
			{SchemaQueryPart::buildQueryOriginalAnalyzed( "/query")},
			{SchemaQueryPart::evaluateQuery( "/query")}
		}
	) {}
};

class Schema_Storage_GET :public papuga::RequestAutomaton, public SchemaQueryPart
{
public:
	Schema_Storage_GET() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		{
			{"result", { {"/query", "ranklist", "ranklist", '!'} }}
		},
		{},
		{
			{SchemaQueryEvalDeclPart::defineQueryEval( "/query/eval")},
			{SchemaQueryPart::createQuery( "/query")},
			{SchemaQueryPart::buildQueryAnalyzed( "/query")},
			{SchemaQueryPart::evaluateQuery( "/query")}
		}
	) {}
};

}}//namespace
#endif


