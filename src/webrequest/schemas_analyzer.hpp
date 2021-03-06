/*
 * Copyright (c) 2018 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* \brief Schema for document analysis
 * @file schemas_document.hpp
 */
#ifndef _STRUS_WEBREQUEST_SCHEMAS_ANALYZER_HPP_INCLUDED
#define _STRUS_WEBREQUEST_SCHEMAS_ANALYZER_HPP_INCLUDED
#include "schemas_base.hpp"
#include "schemas_expression_decl.hpp"
#include "schemas_query_decl.hpp"
#include "schemas_analyzer_decl.hpp"

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class Schema_Context_PUT_DocumentAnalyzer :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_Context_PUT_DocumentAnalyzer() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, itemName, true/*strict*/, false/*exclusive*/,
		{/*env*/},
		{/*result*/},
		{/*inherit*/},
		{/*input*/
			{SchemaAnalyzerPart::defineDocumentAnalyzer( "/docanalyzer")}
		}
	) {}
};

class Schema_Context_POST_DocumentAnalyzer :public Schema_Context_PUT_DocumentAnalyzer
{
public:
	Schema_Context_POST_DocumentAnalyzer() :Schema_Context_PUT_DocumentAnalyzer(){}
};

class Schema_Context_PUT_QueryAnalyzer :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_Context_PUT_QueryAnalyzer() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, itemName, true/*strict*/, false/*exclusive*/,
		{/*env*/},
		{/*result*/},
		{/*inherit*/
			{"vstorage","/qryanalyzer/include/vstorage()",false/*not required*/}
		},
		{/*input*/
			{SchemaAnalyzerPart::defineQueryAnalyzer( "/qryanalyzer")}
		}
	) {}
};

class Schema_Context_POST_QueryAnalyzer :public Schema_Context_PUT_QueryAnalyzer
{
public:
	Schema_Context_POST_QueryAnalyzer() :Schema_Context_PUT_QueryAnalyzer(){}
};


class Schema_QueryAnalyzer_GET :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_QueryAnalyzer_GET() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, itemName, true/*strict*/, false/*exclusive*/,
		{/*env*/},
		{/*result*/
			{"query", {
				{SchemaQueryDeclPart::resultQuery( "/query")}}
		}},
		{/*inherit*/
			{"vstorage","/query/include/vstorage()",false/*not required*/}
		},
		{
			{SchemaAnalyzerPart::defineQueryAnalyzer( "/query/analyzer")},	//... inherited or declared
			{"/query/analyzer", '?'},

			{SchemaQueryDeclPart::declareQuery( "/query")},
			{SchemaQueryDeclPart::analyzeQuery( "/query")}
		}
	) {}
};

}}//namespace
#endif


