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
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, itemName, true/*strict*/, false/*exclusive*/,
		{/*env*/{"path", EnvFormat, "storage/{id}/{name}"}},
		{/*result*/},
		{/*inherit*/
			{"qryeval","/storage/include/qryeval()",false/*not required*/},
			{"qryanalyzer","/qryeval/include/analyzer()",false/*not required*/}
		},
		{/*input*/
			{SchemaStoragePart::defineStorage("/storage")},
			{"/storage", 0, "context", C::createStorage(), {{StorageConfig}} },
			{"/storage", "storage", "context", C::createStorageClient(), {{StorageConfig}} }
		}
	) {}
};

class Schema_Context_PUT_Storage :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	typedef bindings::method::Context C;

	Schema_Context_PUT_Storage() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, itemName, true/*strict*/, false/*exclusive*/,
		{/*env*/{"path", EnvFormat, "storage/{id}/{name}"}},
		{/*result*/},
		{/*inherit*/
			{"qryeval","/storage/include/qryeval()",false/*not required*/},
			{"qryanalyzer","/qryeval/include/analyzer()",false/*not required*/}
		},
		{/*input*/
			{SchemaStoragePart::defineStorage("/storage")},
			{"/", "storage", "context", C::createStorageClient(), {{StorageConfig}} }
		}
	) {}
};

class Schema_Storage_PUT :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	typedef bindings::method::StorageClient S;

	Schema_Storage_PUT() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, itemName, true/*strict*/, true/*exclusive*/,
		{/*env*/{"path", EnvFormat, "storage/{id}/{name}"}},
		{/*result*/},
		{/*inherit*/
			{"qryeval","/storage/include/qryeval()",false/*not required*/},
			{"qryanalyzer","/qryeval/include/analyzer()",false/*not required*/}
		},
		{/*input*/
			{SchemaStoragePart::defineStorage("/storage")},
			{"/", "storage", "context", S::reload(), {{StorageConfig}} }
		}
	) {}
};

class Schema_Storage_PATCH :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	typedef bindings::method::StorageClient S;

	Schema_Storage_PATCH() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, itemName, true/*strict*/, true/*exclusive*/,
		{/*env*/{"path", EnvFormat, "storage/{id}/{name}"}},
		{/*result*/},
		{/*inherit*/
			{"qryeval","/storage/include/qryeval()",false/*not required*/},
			{"qryanalyzer","/qryeval/include/analyzer()",false/*not required*/}
		},
		{/*input*/
			{SchemaStoragePart::defineStorage("/storage")},
			{"/", "storage", "context", S::patch(), {{StorageConfig}} }
		}
	) {}
};

class Schema_Context_DELETE_POST_Storage :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	typedef bindings::method::Context C;

	Schema_Context_DELETE_POST_Storage() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, itemName, true/*strict*/, false/*exclusive*/,
		{/*env*/{"path", EnvFormat, "storage/{id}/{name}"}},
		{/*result*/},
		{/*inherit*/},
		{/*input*/
			{SchemaStoragePart::defineDeleteStorage("/storage")},
			{"/storage", 0, "context", C::destroyStorage(), {{StorageConfig}} }
		}
	) {}
};

class Schema_Storage_GET :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_Storage_GET() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, itemName, true/*strict*/, false/*exclusive*/,
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
	typedef bindings::method::StorageTransaction T;

	Schema_StorageTransaction_PUT() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, itemName, true/*strict*/, false/*exclusive*/,
		{/*env*/},
		{/*result*/},
		{/*inherit*/},
		{/*input*/
			{SchemaAnalyzerPart::defineDocumentAnalyzed( "/storage/document")},
			{"/storage", DocumentDefList, {
					{DocumentDef,'*'}
				}
			},
			{"/storage/document", "", "transaction", T::insertDocument(), {{DocumentId},{DocumentDef}} },
		}
	) {}
};

}}//namespace
#endif


