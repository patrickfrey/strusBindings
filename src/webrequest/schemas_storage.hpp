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

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class Schema_Context_CREATE_Storage :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_Context_CREATE_Storage() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		NULL/*resultname*/,{},
		{
			{"/storage/path", "()", StoragePath, papuga_TypeString, "strus/storage"},
			{"/storage/metadata/name", "()", StorageMetadataName, papuga_TypeString, "doclen"},
			{"/storage/metadata/type", "()", StorageMetadataType, papuga_TypeString, "INT32"},
			{"/storage/metadata", StorageMetadata, {{"name", StorageMetadataName}, {"type", StorageMetadataType}} },
			{"/storage/acl", "()", StorageEnableAcl, papuga_TypeBool, "true"},
			{"/storage/compression", "()", StorageEnableCompression, papuga_TypeBool, "true"},
			{"/storage/cachedterms", "()", StorageCachedTerms, papuga_TypeString, "/srv/strus/termlist.txt"},
			{"/storage/cache", "()", StorageLruCacheSize, papuga_TypeString, "200M"},
			{"/storage/max_open_files", "()", StorageMaxNofOpenFiles, papuga_TypeInt, "128"},
			{"/storage/write_buffer_size", "()", StorageWriteBufferSize, papuga_TypeString, "4M"},
			{"/storage/block_size", "()", StorageBlockSize, papuga_TypeString, "4K"},
			{"/storage", StorageConfig, {
					{"path", StoragePath},
					{"cachedterms", StorageCachedTerms, '?'},
					{"compression", StorageEnableCompression, '?'},
					{"cache", StorageLruCacheSize, '?'},
					{"max_open_files", StorageMaxNofOpenFiles, '?'},
					{"write_buffer_size", StorageWriteBufferSize, '?'},
					{"block_size", StorageBlockSize, '?'},
					{"metadata", StorageMetadata, '*'},
					{"acl", StorageEnableAcl, '?'}
				}
			},
			{"/storage", "_success", "context", bindings::method::Context::createStorage(), {{StorageConfig}} },
			{"/storage", "storage", "context", bindings::method::Context::createStorageClient(), {{StorageConfig}} }
		}
	) {}
};

class Schema_Context_DELETE_Storage :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_Context_DELETE_Storage() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		NULL/*resultname*/,{},
		{
			{"/storage/path", "()", StoragePath, papuga_TypeString, "strus/storage"},
			{"/storage", StorageConfig, {
					{"path", StoragePath}
				}
			},
			{"/storage", "success", "context", bindings::method::Context::destroyStorage(), {{StorageConfig}} }
		}
	) {}
};

class Schema_Context_INIT_Storage :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_Context_INIT_Storage() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		NULL/*resultname*/,{},
		{
			{"/storage/path", "()", StoragePath, papuga_TypeString, "strus/storage"},
			{"/storage/cachedterms", "()", StorageCachedTerms, papuga_TypeString, "/srv/strus/termlist.txt"},
			{"/storage/cache", "()", StorageLruCacheSize, papuga_TypeString, "200M"},
			{"/storage/max_open_files", "()", StorageMaxNofOpenFiles, papuga_TypeInt, "128"},
			{"/storage/write_buffer_size", "()", StorageWriteBufferSize, papuga_TypeString, "4M"},
			{"/storage/block_size", "()", StorageBlockSize, papuga_TypeString, "4K"},
			{"/storage", StorageConfig, {
					{"path", StoragePath},
					{"cachedterms", StorageCachedTerms, '?'},
					{"cache", StorageLruCacheSize, '?'},
					{"max_open_files", StorageMaxNofOpenFiles, '?'},
					{"write_buffer_size", StorageWriteBufferSize, '?'},
					{"block_size", StorageBlockSize, '?'},
				}
			},
			{"/", "storage", "context", bindings::method::Context::createStorageClient(), {{StorageConfig}} }
		}
	) {}
};

}}//namespace
#endif


