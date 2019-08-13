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
				{"path", DatabasePath},
				{"cache", DatabaseLruCacheSize, '?'},
				{"compression", DatabaseEnableCompression, '?'},
				{"autocompact", DatabaseEnableAutoCompact, '?'},
				{"max_open_files", DatabaseMaxNofOpenFiles, '?'},
				{"write_buffer_size", DatabaseWriteBufferSize, '?'},
				{"block_size", DatabaseBlockSize, '?'},
				{"cachedterms", StorageCachedTerms, '?'},
				{"metadata", StorageMetadata, '*'},
				{"acl", StorageEnableAcl, '?'}
			}}
		});
	}
};

class Schema_Context_CREATE_Storage :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_Context_CREATE_Storage() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, true/*strict*/,
		{},
		{},
		{
			{SchemaStoragePart::defineStorage("/storage")},
			{"/storage", "_success", "context", bindings::method::Context::createStorage(), {{StorageConfig}} },
			{"/storage", "storage", "context", bindings::method::Context::createStorageClient(), {{StorageConfig}} }
		}
	) {}
};

class Schema_Context_INIT_Storage :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_Context_INIT_Storage() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, true/*strict*/,
		{},
		{},
		{
			{SchemaStoragePart::defineStorage("/storage")},
			{"/", "storage", "context", bindings::method::Context::createStorageClient(), {{StorageConfig}} }
		}
	) {}
};

class Schema_Context_DELETE_Storage :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_Context_DELETE_Storage() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, true/*strict*/,
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

class Schema_Storage_GET :public papuga::RequestAutomaton
{
public:
	Schema_Storage_GET() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, true/*strict*/,
		{
			{"result", { {"/query", "ranklist", "ranklist", '!'} }}
		},
		{},
		{
			{SchemaQueryDeclPart::createQuery( "/query")},
			{SchemaQueryDeclPart::buildQueryAnalyzed( "/query")},
			{SchemaQueryDeclPart::evaluateQuery( "/query")}
		}
	) {}
};

class Schema_StorageTransaction_PUT :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_StorageTransaction_PUT() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, true/*strict*/,
		{},
		{},
		{
			{"/storage/document/id", "()", DocumentId, papuga_TypeString, "/company/ACME"},
			{"/storage/document/doctype", "()", SubDocumentName, papuga_TypeString, "article"},
			{"/storage/document/attribute/name", "()", DocumentAttributeName, papuga_TypeString, "title"},
			{"/storage/document/attribute/value", "()", DocumentAttributeValue, papuga_TypeString, "the king is back"},
			{"/storage/document/attribute", DocumentAttributeDef, {
					{DocumentAttributeName},
					{DocumentAttributeValue}
				}
			},

			{"/storage/document/metadata/name", "()", DocumentMetaDataName, papuga_TypeString, "title"},
			{"/storage/document/metadata/value", "()", DocumentMetaDataValue, papuga_TypeString, "12;32443;4324"},
			{"/storage/document/metadata", DocumentMetaDataDef, {
					{DocumentMetaDataName},
					{DocumentMetaDataValue}
				}
			},

			{"/storage/document/{forwardindex,searchindex}/type", "()", DocumentFeatureType, papuga_TypeString, "word"},
			{"/storage/document/{forwardindex,searchindex}/value", "()", DocumentFeatureValue, papuga_TypeString, "hello"},
			{"/storage/document/{forwardindex,searchindex}/pos", "()", DocumentFeaturePos, papuga_TypeInt, "1;3;13;3452"},
			{"/storage/document/{forwardindex,searchindex}/len", "()", DocumentFeatureLen, papuga_TypeInt, "1;2;5"},

			{"/storage/document/forwardindex", DocumentForwardIndexFeatureDef, {
					{DocumentFeatureType},
					{DocumentFeatureValue},
					{DocumentFeaturePos},
					{DocumentFeatureLen,'?'}
				}
			},
			{"/storage/document/searchindex", DocumentSearchIndexFeatureDef, {
					{DocumentFeatureType},
					{DocumentFeatureValue},
					{DocumentFeaturePos},
					{DocumentFeatureLen,'?'}
				}
			},
			{"/storage/document/access", "()", DocumentAccess, papuga_TypeString, "muller;all;doe"},
			{"/storage/document", DocumentDef, {
					{"docid",DocumentId,'!'},
					{"doctype",SubDocumentName,'?'},
					{"searchindex",DocumentSearchIndexFeatureDef,'*'},
					{"forwardindex",DocumentForwardIndexFeatureDef,'*'},
					{"metadata",DocumentMetaDataDef,'*'},
					{"attribute",DocumentAttributeDef,'*'},
					{"access",DocumentAccess,'*'}
				}
			},
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


