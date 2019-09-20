/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* \brief Schemas for storages
 * @file schemas_storage.hpp
 */
#ifndef _STRUS_WEBREQUEST_SCHEMAS_STATISTICS_HPP_INCLUDED
#define _STRUS_WEBREQUEST_SCHEMAS_STATISTICS_HPP_INCLUDED
#include "schemas_base.hpp"
#include "schemas_expression_decl.hpp"

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class SchemaStatisticsPart :public AutomatonNameSpace
{
public:
	static papuga::RequestAutomaton_NodeList evaluateTermStatistics( const char* rootexpr)
	{
		typedef bindings::method::StatisticsMap S;
		return papuga::RequestAutomaton_NodeList( rootexpr,{
			{"//term", "_termstats", "statserver", S::df(), {{TermType}, {TermValue}}},
		});
	}

	static papuga::RequestAutomaton_NodeList evaluateGlobalStatistics( const char* rootexpr)
	{
		typedef bindings::method::StatisticsMap S;
		return papuga::RequestAutomaton_NodeList( rootexpr,{
			{"", "_nofdocs", "statserver", S::nofDocuments(), {}}
		});
	}
};

class Schema_Context_PUT_StatisticsServer :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_Context_PUT_StatisticsServer() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, true/*strict*/,
		{
			{"statquery", "PUT~statistics", "GET", "storage", "statistics/snapshot", {}}
		},
		{},
		{
			{"/statserver/storage", "()", StatisticsStorageServer, papuga_TypeString, "example.com:7184/storage/test"},
			{"/statserver", "", "storage", StatisticsStorageServer, '*'},
			{"/statserver/blocks", "()", StatisticsMapBlocks, papuga_TypeString, "100K"},
			{"/statserver/proc", "()", StatisticsProc, papuga_TypeString, "std"},
			{"/statserver", StatisticsMapConfig, {
					{"proc", StatisticsProc, '?'},
					{"blocks", StatisticsMapBlocks, '?'},
				}
			},
			{"/", "statserver", "context", bindings::method::Context::createStatisticsMap(), {{StatisticsMapConfig}} }
		}
	) {}
};

class Schema_StatisticsServer_PUT_statistics :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_StatisticsServer_PUT_statistics() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, true/*strict*/,
		{},
		{},
		{
			{"/storage/value", "()", StatisticsBlob, papuga_TypeString, "AAAABwAKZ9h..."},
			{"/storage/value", 0/*result*/, "statserver", bindings::method::StatisticsMap::processStatisticsMessage(), {{StatisticsBlob}} }
		}
	) {}
};

class Schema_StatisticsServer_GET :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_StatisticsServer_GET() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, true/*strict*/,
		{{"statistics", {
			{"/query/feature/analyzed//term", "termstats", true},
			{"/query", "globalstats", false},
			{"/query", "nofdocs", "_nofdocs", '!'},
			{"/query/feature/analyzed//term/type", "type", TermType},
			{"/query/feature/analyzed//term/value", "value", TermValue},
			{"/query/feature/analyzed//term", "df", "_termstats", '!'}
		}}},
		{},
		{
			{"/query/feature/analyzed", "()", TermExpression, papuga_TypeVoid, NULL},
			{SchemaExpressionPart::declareTermExpression( "/query/feature/analyzed")},
			{SchemaStatisticsPart::evaluateTermStatistics( "/query/feature/analyzed")},
			{SchemaStatisticsPart::evaluateGlobalStatistics( "/query")}
		}
	) {}
};

}}//namespace
#endif

