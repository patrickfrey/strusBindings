/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* \brief Schemes for storages
 * @file schemes_storage.hpp
 */
#ifndef _STRUS_WEBREQUEST_SCHEMES_STORAGE_HPP_INCLUDED
#define _STRUS_WEBREQUEST_SCHEMES_STORAGE_HPP_INCLUDED
#include "schemes_base.hpp"

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class Scheme_Context_PUT_Storage :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Scheme_Context_PUT_Storage() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		NULL/*resultname*/,{},
		{
			{"/storage/path", "()", StoragePath},
			{"/storage/metadata/name", "()", StorageMetadataName},
			{"/storage/metadata/type", "()", StorageMetadataType},
			{"/storage/metadata", StorageMetadata, {{"name", StorageMetadataName}, {"type", StorageMetadataType}} },
			{"/storage/acl", "()", StorageEnableAcl},
			{"/storage/compression", "()", StorageEnableCompression},
			{"/storage/cachedterms", "()", StorageCachedTerms},
			{"/storage/cache", "()", StorageLruCacheSize},
			{"/storage/max_open_files", "()", StorageMaxNofOpenFiles},
			{"/storage/write_buffer_size", "()", StorageWriteBufferSize},
			{"/storage/block_size", "()", StorageBlockSize},
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

class Scheme_Context_DELETE_Storage :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Scheme_Context_DELETE_Storage() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		NULL/*resultname*/,{},
		{
			{"/storage/path", "()", StoragePath},
			{"/storage", StorageConfig, {
					{"path", StoragePath}
				}
			},
			{"/storage", "success", "context", bindings::method::Context::destroyStorage(), {{StorageConfig}} }
		}
	) {}
};

class Scheme_Context_INIT_Storage :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Scheme_Context_INIT_Storage() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		NULL/*resultname*/,{},
		{
			{"/storage/path", "()", StoragePath},
			{"/storage/cachedterms", "()", StorageCachedTerms},
			{"/storage/compression", "()", StorageEnableCompression},
			{"/storage/cache", "()", StorageLruCacheSize},
			{"/storage/max_open_files", "()", StorageMaxNofOpenFiles},
			{"/storage/write_buffer_size", "()", StorageWriteBufferSize},
			{"/storage/block_size", "()", StorageBlockSize},
			{"/storage", StorageConfig, {
					{"path", StoragePath},
					{"cachedterms", StorageCachedTerms, '?'},
					{"compression", StorageEnableCompression, '?'},
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


