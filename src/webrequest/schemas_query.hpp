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

class Schema_Context_INIT_QueryEval :public papuga::RequestAutomaton
{
public:
	Schema_Context_INIT_QueryEval() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, true/*strict*/,
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
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, true/*strict*/,
		{
			{"result", { {"/query", "ranklist", "ranklist", '!'} }}
		},
		{},
		{
			{SchemaQueryDeclPart::createQuery( "/query")},
			{SchemaQueryDeclPart::buildQueryOriginalAnalyzed( "/query")},
			{SchemaQueryDeclPart::evaluateQuery( "/query")}
		}
	) {}
};

}}//namespace
#endif


