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
	static papuga::RequestAutomaton_NodeList buildQueryOriginal()
	{
		typedef bindings::method::Query Q;
		typedef bindings::method::QueryAnalyzer A;
		return {
			{SchemaQueryDeclPart::declareFeature()},
			{"/query/feature", "_feature", "qryanalyzer", A::analyzeSingleTermExpression(), {{TermExpression, '!', 2/*tag diff*/}} },
			{"/query/feature", 0, "query", Q::addFeature(), {{FeatureSet}, {"_feature"}, {FeatureWeight, '?'}} },

			{SchemaQueryDeclPart::declareMetaData()},
			{"/query/restriction", "_condition", "qryanalyzer", A::analyzeMetaDataExpression(), {{MetaDataCondition, '*'}} },
			{"/query/restriction", 0, "query", Q::addMetaDataRestriction(),  {"_condition"} }
		};
	}

	static papuga::RequestAutomaton_NodeList buildQueryAnalyzed()
	{
		typedef bindings::method::Query Q;
		return {
			{SchemaQueryDeclPart::declareFeature()},
			{"/query/feature", 0, "query", Q::addFeature(), {{FeatureSet}, {TermExpression, '!', 2/*tag diff*/}, {FeatureWeight, '?'}} },

			{SchemaQueryDeclPart::declareMetaData()},
			{"/query/restriction", 0, "query", Q::addMetaDataRestriction(),  {MetaDataCondition} }
		};
	}
};

class Schema_Storage_QRYORG :public papuga::RequestAutomaton
{
public:
	Schema_Storage_QRYORG() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		{
			{"result", { {"/query", "ranklist", "ranklist", '!'} }}
		},
		{},
		{
			{SchemaQueryEvalDeclPart::defineQueryEval()},
			{SchemaAnalyzerPart::defineQueryAnalyzer( "/query/analyzer")},
			{"/query", "query", "queryeval", bindings::method::QueryEval::createQuery(), {{"storage"}} },
			{SchemaQueryPart::buildQueryOriginal()},
			{SchemaQueryDeclPart::defineRankingParameter()},
			{"/query", "ranklist", "query", bindings::method::Query::evaluate(), {} }
		}
	) {}
};

class Schema_Storage_QRYANA :public papuga::RequestAutomaton, public SchemaQueryPart
{
public:
	Schema_Storage_QRYANA() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		{
			{"result", { {"/query", "ranklist", "ranklist", '!'} }}
		},
		{},
		{
			{SchemaQueryEvalDeclPart::defineQueryEval()},
			{"/query", "query", "queryeval", bindings::method::QueryEval::createQuery(), {{"storage"}} },
			{SchemaQueryPart::buildQueryAnalyzed()},
			{SchemaQueryDeclPart::defineStatistics()},
			{SchemaQueryDeclPart::defineRankingParameter()},
			{"/query", "ranklist", "query", bindings::method::Query::evaluate(), {} }
		}
	) {}
};

}}//namespace
#endif


