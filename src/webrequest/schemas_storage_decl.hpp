/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* \brief Partial schema definition to include for storages
 * @file schemas_storage.hpp
 */
#ifndef _STRUS_WEBREQUEST_SCHEMAS_STORAGE_DECL_HPP_INCLUDED
#define _STRUS_WEBREQUEST_SCHEMAS_STORAGE_DECL_HPP_INCLUDED
#include "schemas_base.hpp"

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class SchemaStoragePart :public AutomatonNameSpace
{
public:
	static papuga::RequestAutomaton_NodeList defineDeleteStorage( const char* rootexpr)
	{
		return papuga::RequestAutomaton_NodeList( rootexpr,
		{
			{"path", "()", DatabasePath, papuga_TypeString, "strus/storage"},
			{"path", "", "path", DatabasePath, '?'},
			{"database", "()", DatabaseEngine, papuga_TypeString, "std"},
			{"", StorageConfig, {
				{"path", "path"},
				{"database", DatabaseEngine, '?'}
			}}
		});
	}

	static papuga::RequestAutomaton_NodeList defineStorage( const char* rootexpr)
	{
		return papuga::RequestAutomaton_NodeList( rootexpr,
		{
			{"path", "()", DatabasePath, papuga_TypeString, "strus/storage"},
			{"path", "", "path", DatabasePath, '?'},
			{"database", "()", DatabaseEngine, papuga_TypeString, "std"},
			{"statsproc", "()", StatisticsProc, papuga_TypeString, "std"},
			{"autocompact", "()", DatabaseEnableAutoCompact, papuga_TypeBool, "true"},
			{"acl", "()", StorageEnableAcl, papuga_TypeBool, "true"},
			{"cachedterms", "()", StorageCachedTerms, papuga_TypeString, "/srv/strus/termlist.txt"},
			{"cache", "()", DatabaseLruCacheSize, papuga_TypeString, "200M"},
			{"compression", "()", DatabaseEnableCompression, papuga_TypeBool, "true"},
			{"max_open_files", "()", DatabaseMaxNofOpenFiles, papuga_TypeInt, "128"},
			{"write_buffer_size", "()", DatabaseWriteBufferSize, papuga_TypeString, "4M"},
			{"block_size", "()", DatabaseBlockSize, papuga_TypeString, "4K"},
			{"", StorageConfig, {
				{"path", "path"},
				{"database", DatabaseEngine, '?'},
				{"statsproc", StatisticsProc, '?'},
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

	static papuga::RequestAutomaton_NodeList defineMetaDataTableCommand( const char* rootexpr)
	{
		return papuga::RequestAutomaton_NodeList( rootexpr,
		{
			{"op", "()", StorageMetadataTableOperation, papuga_TypeString, "add;rename;remove;clear;alter"},
			{"name", "()", StorageMetadataName, papuga_TypeString, "doclen"},
			{"type", "()", StorageMetadataType, papuga_TypeString, "UINT16;INT32;INT8;FLOAT32"},
			{"oldname", "()", StorageMetadataOldname, papuga_TypeString, "doclen"},
			{"", StorageMetadataTableCommand, {
				{"op", StorageMetadataTableOperation, '!'},
				{"name", StorageMetadataName, '?'},
				{"type", StorageMetadataType, '?'},
				{"oldname", StorageMetadataOldname, '?'}
			}},
			
		});
	}
};

}}//namespace
#endif


