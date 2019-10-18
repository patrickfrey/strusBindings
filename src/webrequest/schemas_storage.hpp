/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* \brief Schemas for storages
 * @file schemas_storage.hpp
 */
#ifndef _STRUS_WEBREQUEST_SCHEMAS_STORAGE_HPP_INCLUDED
#define _STRUS_WEBREQUEST_SCHEMAS_STORAGE_HPP_INCLUDED
#include "schemas_base.hpp"
#include "schemas_query_decl.hpp"
#include "schemas_queryeval_decl.hpp"
#include "schemas_analyzer_decl.hpp"
#include "schemas_storage_decl.hpp"

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class Schema_Context_POST_Storage :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	typedef bindings::method::Context C;
	Schema_Context_POST_Storage() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, itemName, true/*strict*/,
		{/*env*/{"path", EnvFormat, "storage/{id}/{name}"}},
		{/*result*/},
		{/*inherit*/
			{"qryeval","/storage/include/qryeval()",false/*not required*/},
			{"qryanalyzer","/qryeval/include/analyzer()",false/*not required*/}
		},
		{/*input*/
			{SchemaStoragePart::defineStorage("/storage")},
			{"/storage", "_success", "context", C::createStorage(), {{StorageConfig}} },
			{"/storage", "storage", "context", C::createStorageClient(), {{StorageConfig}} }
		}
	) {}
};

class Schema_Context_PUT_Storage :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_Context_PUT_Storage() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, itemName, true/*strict*/,
		{/*env*/{"path", EnvFormat, "storage/{id}/{name}"}},
		{/*result*/},
		{/*inherit*/
			{"qryeval","/storage/include/qryeval()",false/*not required*/},
			{"qryanalyzer","/qryeval/include/analyzer()",false/*not required*/}
		},
		{/*input*/
			{SchemaStoragePart::defineStorage("/storage")},
			{"/", "storage", "context", bindings::method::Context::createStorageClient(), {{StorageConfig}} }
		}
	) {}
};

class Schema_Context_DELETE_POST_Storage :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_Context_DELETE_POST_Storage() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, itemName, true/*strict*/,
		{/*env*/},
		{/*result*/},
		{/*inherit*/},
		{/*input*/
			{"/storage/path", "()", DatabasePath, papuga_TypeString, "strus/storage"},
			{"/storage", StorageConfig, {
					{"path", DatabasePath}
				}
			},
			{"/storage", "success", "context", bindings::method::Context::destroyStorage(), {{StorageConfig}} }
		}
	) {}
};

class Schema_Storage_GET :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_Storage_GET() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, itemName, true/*strict*/,
		{/*env*/},
		{/*result*/
			{"queryresult", { {"/query", "ranklist", "ranklist", '!'} }},
			{"statistics", { {"/statistics", "_blob", "statistics", '!'} }}
		},
		{/*inherit*/
			{"qryeval","/storage/include/qryeval()",false/*not required*/},
			{"qryanalyzer","/qryeval/include/analyzer()",false/*not required*/}
		},
		{/*input*/
			{SchemaQueryEvalDeclPart::defineQueryEval( "/query/eval")},	//... inherited or declared
			{SchemaAnalyzerPart::defineQueryAnalyzer( "/query/analyzer")},	//... inherited or declared

			{SchemaQueryDeclPart::declareQuery( "/query", "content")},
			{SchemaQueryDeclPart::analyzeQuery( "/query")},
			{SchemaQueryDeclPart::defineQuery( "/query")},
			{SchemaQueryDeclPart::evaluateQuery( "/query")},

			{SchemaStoragePart::defineStatisticsQuery( "/statistics")}
		}
	) {}
};

class Schema_StorageTransaction_PUT :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_StorageTransaction_PUT() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, itemName, true/*strict*/,
		{/*env*/},
		{/*result*/},
		{/*inherit*/},
		{/*input*/
			{SchemaAnalyzerPart::defineDocumentAnalyzed( "/storage/document")},
			{"/storage", DocumentDefList, {
					{DocumentDef,'*'}
				}
			},
			{"/storage/document", "", "transaction", bindings::method::StorageTransaction::insertDocument(), {{DocumentId},{DocumentDef}} },
		}
	) {}
};

}}//namespace
#endif


