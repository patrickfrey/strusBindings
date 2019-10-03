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

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class Schema_Context_POST_DistQueryEval :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_Context_POST_DistQueryEval() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, true/*strict*/,
		{},
		{
			{"qryanalyzer","/distqryeval/include/analyzer()",false/*not required*/},
			{"vstorage","/distqryeval/include/vstorage()",false/*not required*/}
		},
		{
			{"/distqryeval/storage", "()", DistQueryEvalStorageServer, papuga_TypeString, "example.com:7184/storage/test"},
			{"/distqryeval", "", "storage", DistQueryEvalStorageServer, '*'},
			{"/distqryeval/statserver", "()", DistQueryEvalStatisticsServer, papuga_TypeString, "example.com:7184/statserver/test"},
			{"/distqryeval", "", "statserver", DistQueryEvalStatisticsServer, '!'},

			{SchemaAnalyzerPart::defineQueryAnalyzer( "/query/analyzer")},
			{"/query/analyzer", '?'}
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
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, true/*strict*/,
		{
			{"query", "SET~querystats", "GET", "statserver", "", {
				{"/query/feature", "feature", true},
				{"/query/{feature,sentence}/content", "analyzed", "_analyzed", '*'}
			}},
			{"query", "SET~ranklist", "GET", "qryeval", "", {"termstats","globalstats"}, {
				{SchemaQueryDeclPart::resultElementsQueryAnalyzer( "/query")}
			}},
			{"queryresult", {"ranklist"}, {}}
		},
		{
			{"qryanalyzer","/query/include/analyzer()",false/*not required*/},
			{"vstorage","/query/include/vstorage()",false/*not required*/}
		},
		{
			{"/query/server/storage", "()", DistQueryEvalStorageServer, papuga_TypeString, "example.com:7184/storage/test"},
			{"/query/server", "", "storage", DistQueryEvalStorageServer, '*'},
			{"/query/server/statserver", "()", DistQueryEvalStatisticsServer, papuga_TypeString, "example.com:7184/statserver/test"},
			{"/query/server", "", "statserver", DistQueryEvalStatisticsServer, '!'},

			{SchemaAnalyzerPart::defineQueryAnalyzer( "/query/analyzer")},
			{"/query/analyzer", '?'},
		
			{SchemaQueryDeclPart::analyzeQuerySchemaOutput( "/query")},
		}
	) {}
};

class Schema_DistQueryEval_SET_querystats :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_DistQueryEval_SET_querystats() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, true/*strict*/,
		{},
		{},
		{
			{SchemaQueryDeclPart::defineStatistics("/statitics")},
			{"/statitics", "", "termstats", TermStats, '!'},
			{"/statitics", "", "globalstats", GlobalStats, '!'}
		}
	) {}
};

class Schema_DistQueryEval_SET_ranklist :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_DistQueryEval_SET_ranklist() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, true/*strict*/,
		{},
		{},
		{
			{SchemaQueryDeclPart::defineRanklist( "/queryresult/ranklist")},
		}
	) {}
};

}}//namespace
#endif


