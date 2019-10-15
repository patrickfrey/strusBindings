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

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class SchemaStoragePart :public AutomatonNameSpace
{
public:
	static papuga::RequestAutomaton_NodeList defineStorage( const char* rootexpr)
	{
		return papuga::RequestAutomaton_NodeList( rootexpr,
		{
			{"path", "()", DatabasePath, papuga_TypeString, "strus/storage"},
			{"database", "()", DatabaseEngine, papuga_TypeString, "std"},
			{"statsproc", "()", StatisticsProc, papuga_TypeString, "std"},
			{"metadata/name", "()", StorageMetadataName, papuga_TypeString, "doclen"},
			{"metadata/type", "()", StorageMetadataType, papuga_TypeString, "INT32"},
			{"metadata", StorageMetadata, {{"name", StorageMetadataName}, {"type", StorageMetadataType}} },
			{"autocompact", "()", DatabaseEnableAutoCompact, papuga_TypeBool, "true"},
			{"acl", "()", StorageEnableAcl, papuga_TypeBool, "true"},
			{"cachedterms", "()", StorageCachedTerms, papuga_TypeString, "/srv/strus/termlist.txt"},
			{"cache", "()", DatabaseLruCacheSize, papuga_TypeString, "200M"},
			{"compression", "()", DatabaseEnableCompression, papuga_TypeBool, "true"},
			{"max_open_files", "()", DatabaseMaxNofOpenFiles, papuga_TypeInt, "128"},
			{"write_buffer_size", "()", DatabaseWriteBufferSize, papuga_TypeString, "4M"},
			{"block_size", "()", DatabaseBlockSize, papuga_TypeString, "4K"},
			{"", StorageConfig, {
				{"path", DatabasePath, '!'},
				{"database", DatabaseEngine, '?'},
				{"statsproc", StatisticsProc, '?'},
				{"metadata", StorageMetadata, '*'},
				{"autocompact", DatabaseEnableAutoCompact, '?'},
				{"acl", StorageEnableAcl, '?'},
				{"cachedterms", StorageCachedTerms, '?'},
				{"cache", DatabaseLruCacheSize, '?'},
				{"compression", DatabaseEnableCompression, '?'},
				{"max_open_files", DatabaseMaxNofOpenFiles, '?'},
				{"write_buffer_size", DatabaseWriteBufferSize, '?'},
				{"block_size", DatabaseBlockSize, '?'}
			}}
		});
	}

	static papuga::RequestAutomaton_NodeList defineStatisticsQuery( const char* rootexpr)
	{
		typedef bindings::method::StorageClient S;
		return papuga::RequestAutomaton_NodeList( rootexpr,
		{
			{"snapshot", "_blob", "storage", S::getAllStatistics(), {}}
		});
	}
};

class Schema_Context_POST_Storage :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	typedef bindings::method::Context C;
	Schema_Context_POST_Storage() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, itemName, true/*strict*/,
		{},
		{//Inherited:
			{"qryeval","/storage/include/qryeval()",false/*not required*/},
			{"qryanalyzer","/qryeval/include/analyzer()",false/*not required*/}
		},
		{
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
		{},
		{//Inherited:
			{"qryeval","/storage/include/qryeval()",false/*not required*/},
			{"qryanalyzer","/qryeval/include/analyzer()",false/*not required*/}
		},
		{
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
		{},
		{},
		{
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
		{
			{"queryresult", { {"/query", "ranklist", "ranklist", '!'} }},
			{"statistics", { {"/statistics", "_blob", "statistics", '!'} }}
		},
		{//Inherited:
			{"qryeval","/storage/include/qryeval()",false/*not required*/},
			{"qryanalyzer","/qryeval/include/analyzer()",false/*not required*/}
		},
		{
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
		{},
		{},
		{
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


