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
#ifndef _STRUS_WEBREQUEST_SCHEMAS_DIST_QUERYEVAL_HPP_INCLUDED
#define _STRUS_WEBREQUEST_SCHEMAS_DIST_QUERYEVAL_HPP_INCLUDED
#include "schemas_base.hpp"
#include "schemas_expression_decl.hpp"
#include "schemas_query_decl.hpp"
#include "schemas_queryeval_decl.hpp"

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class Schema_Context_POST_DistQueryEval :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_Context_POST_DistQueryEval() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, itemName, true/*strict*/, false/*exclusive*/,
		{/*env*/},
		{/*result*/},
		{/*inherit*/},
		{/*input*/
			{"/distqryeval/analyzer", "()", DistQueryEvalAnalyzeServer, papuga_TypeString, "example.com:7191/qryanalyzer/test"},
			{"/distqryeval", "", "qryanalyzer", DistQueryEvalAnalyzeServer, '!'},
			{"/distqryeval/collector", "()", DistQueryEvalCollectServer, papuga_TypeString, "example.com:7184/qryeval/bm25"},
			{"/distqryeval", "", "collector", DistQueryEvalCollectServer, '*'},
			{"/distqryeval/qryeval", "()", DistQueryEvalStorageServer, papuga_TypeString, "example.com:7184/qryeval/test"},
			{"/distqryeval", "", "qryeval", DistQueryEvalStorageServer, '*'},
			{"/distqryeval/statserver", "()", DistQueryEvalStatisticsServer, papuga_TypeString, "example.com:7184/statserver/test"},
			{"/distqryeval", "", "statserver", DistQueryEvalStatisticsServer, '!'},
			{SchemaQueryEvalDeclPart::declareQueryBuilderConfig( "/distqryeval/config")},
			{"/distqryeval", "config", "config", QueryBuilderDef, '?'}
		}
	) {}
};

class Schema_Context_PUT_DistQueryEval :public Schema_Context_POST_DistQueryEval
{
public:
	Schema_Context_PUT_DistQueryEval() :Schema_Context_POST_DistQueryEval(){}
};

class Schema_DistQueryEval_GET :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_DistQueryEval_GET() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, itemName, true/*strict*/, false/*exclusive*/,
		{/*env*/},
		{/*result*/
			{"query", "SET~analysis", "GET", "qryanalyzer", "", {
				{SchemaQueryDeclPart::resultQueryOrig("/query")}
			}},
			{"query", "SET~querystats", "GET", "statserver", "", {"_feature","_restriction"}, {
			}},
			{"query", "SET~collect", "GET", "collector", "", {"_feature","_termstats","_globalstats"}, {
				{{"/query","mergeres", "y", '#'}}
			}},
			{"query", "SET~ranklist", "GET", "qryeval", "", {"_feature","_restriction","_termstats","_globalstats"}, {
				{{"/query","mergeres", "y", '#'}}
			}},
			{"query", "END~ranklist", {}},
			{"queryresult", {"ranklist"}, {}}
		},
		{/*inherit*/
			{"qryanalyzer","/query/include/analyzer()",false/*not required*/},
			{"vstorage","/query/include/vstorage()",false/*not required*/}
		},
		{/*input*/
			{"/query/server/qryeval", "()", DistQueryEvalStorageServer, papuga_TypeString, "example.com:7184/storage/test"},
			{"/query/server", "", "qryeval", DistQueryEvalStorageServer, '*'},
			{"/query/server/statserver", "()", DistQueryEvalStatisticsServer, papuga_TypeString, "example.com:7184/statserver/test"},
			{"/query/server", "", "statserver", DistQueryEvalStatisticsServer, '!'},
			{"/query/server/collector", "()", DistQueryEvalCollectServer, papuga_TypeString, "example.com:7184/qryeval/collector"},
			{"/query/server", "", "collector", DistQueryEvalCollectServer, '!'},

			{SchemaAnalyzerPart::defineQueryAnalyzer( "/query/analyzer")},	//... inherited or declared
			{"/query/analyzer", '?'},

			{SchemaQueryDeclPart::declareQuery( "/query")},
			{SchemaQueryDeclPart::defineQueryBuilder( "/query", "config")},
			{SchemaQueryDeclPart::defineResultMerger( "/query")}
		}
	) {}
};

class Schema_DistQueryEval_SET_analysis :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_DistQueryEval_SET_analysis() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, itemName, true/*strict*/, false/*exclusive*/,
		{/*env*/},
		{/*result*/},
		{/*inherit*/},
		{/*input*/
			{SchemaQueryDeclPart::declareQuery( "/query")},
			{SchemaQueryDeclPart::defineQueryAnalyzed( "/query")},
			{SchemaQueryDeclPart::buildQuery( "/query")}
		}
	) {}
};

class Schema_DistQueryEval_SET_querystats :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_DistQueryEval_SET_querystats() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, itemName, true/*strict*/, false/*exclusive*/,
		{/*env*/},
		{/*result*/},
		{/*inherit*/},
		{/*input*/
			{SchemaQueryDeclPart::declareStatistics("/statistics")},
			{"/statistics", "termstats", "_termstats", TermStats, '*'},
			{"/statistics", "globalstats", "_globalstats", GlobalStats, '!'}
		}
	) {}
};

class Schema_DistQueryEval_SET_collect :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_DistQueryEval_SET_collect() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, itemName, true/*strict*/, false/*exclusive*/,
		{/*env*/},
		{/*result*/},
		{/*inherit*/},
		{/*input*/
			{SchemaQueryDeclPart::declareQueryResult( "/queryresult/ranklist")},
			{SchemaQueryDeclPart::buildQueryFromResult( "/queryresult/ranklist")}
		}
	) {}
};

class Schema_DistQueryEval_SET_ranklist :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	typedef bindings::method::Context C;
	Schema_DistQueryEval_SET_ranklist() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, itemName, true/*strict*/, false/*exclusive*/,
		{/*env*/},
		{/*result*/},
		{/*inherit*/},
		{/*input*/
			{SchemaQueryDeclPart::declareQueryResult( "/queryresult/ranklist")},
			{SchemaQueryDeclPart::addQueryResult( "/queryresult/ranklist")},
		}
	) {}
};

class Schema_DistQueryEval_END_ranklist :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	typedef bindings::method::Context C;
	Schema_DistQueryEval_END_ranklist() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, itemName, true/*strict*/, false/*exclusive*/,
		{/*env*/},
		{/*result*/},
		{/*inherit*/},
		{/*input*/
			{SchemaQueryDeclPart::mergeQueryResults( "/query")}
		}
	) {}
};

}}//namespace
#endif


