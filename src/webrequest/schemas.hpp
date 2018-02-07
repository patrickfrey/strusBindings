/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* \brief Structure to build and map the Result of an XML/JSON request
 * @file requestResult.h
 */
#ifndef _STRUS_WEBREQUEST_SCHEMAS_HPP_INCLUDED
#define _STRUS_WEBREQUEST_SCHEMAS_HPP_INCLUDED
#include "papuga.hpp"
#include "strus/bindingClasses.h"
#include "strus/bindingObjects.h"
#include "strus/bindingMethodIds.hpp"
#include "strus/lib/bindings_description.hpp"

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

enum
{
	NullValue,

	ModuleDir,ModuleName,ResourceDir,WorkDir,ContextConfig,ContextThreads,ContextRpc,ContextTrace,TraceLogType,TraceLogFile,TraceGroupBy,TraceCall,TraceCount,

	StorageConfig, StoragePath, StorageCachedTerms,
	StorageMetadata, StorageMetadataName, StorageMetadataValue,
	StorageEnableAcl, StorageEnableCompression, StorageLruCacheSize,
	StorageMaxNofOpenFiles, StorageWriteBufferSize, StorageBlockSize,

	TermType, TermValue,
};


class SchemaCreateContext :public papuga::RequestAutomaton
{
public:
	SchemaCreateContext() :papuga::RequestAutomaton(
		getStrusClassDefs(), getBindingsInterfaceDescription()->structs,
		"context",
		{
			{"/extensions/directory", "()", ModuleDir},
			{"/extensions/modules", "()", ModuleName},
			{"/data/workdir", "()", WorkDir},
			{"/data/resources", "()", ResourceDir},
			{"/context/trace", "log()", TraceLogType},
			{"/context/trace", "file()", TraceLogFile},
			{"/context/trace", "groupby()", TraceGroupBy},
			{"/context/trace", "call()", TraceCall},
			{"/context/trace", "count()", TraceCount},
			{"/context/trace", ContextTrace, {
							{"log",  TraceLogType, '!'},
							{"file", TraceLogFile, '?'},
							{"groupby", TraceGroupBy, '?'},
							{"call", TraceCall, '?'},
							{"count", TraceCount, '?'}
						}
			},
			{"/context/rpc", "()", ContextRpc},
			{"/context/threads", "()", ContextThreads},
			{"/context", ContextConfig, {
					{"rpc", ContextRpc, '?'},
					{"trace", ContextTrace, '?'},
					{"threads", ContextThreads, '?'}
				}
			},
			{"/", "context", "", bindings::method::Context::constructor(), {{(int)ContextConfig, '?'}} },
			{"/data/workdir", "", "context", bindings::method::Context::defineWorkingDirectory(), {{(int)WorkDir}} },
			{"/data/resources", "", "context", bindings::method::Context::addResourcePath(), {{(int)ResourceDir}} },
			{"/extensions/directory", "", "context", bindings::method::Context::addModulePath(), {{(int)ModuleDir}} },
			{"/extensions/modules", "", "context", bindings::method::Context::loadModule(), {{(int)ModuleName}} },
			{"/", "", "context", bindings::method::Context::endConfig(), {} }
		}
	) {}
};

class SchemaCreateStorage :public papuga::RequestAutomaton
{
public:
	SchemaCreateStorage() :papuga::RequestAutomaton(
		getStrusClassDefs(), getBindingsInterfaceDescription()->structs,
		"storage",
		{
			{"/storage", "path()", StoragePath},
			{"/storage/metadata", "name()", StorageMetadataName},
			{"/storage/metadata", "value()", StorageMetadataValue},
			{"/storage/metadata", StorageMetadata, {{"name", StorageMetadataName}, {"value", StorageMetadataValue}} },
			{"/storage", "acl()", StorageEnableAcl},
			{"/storage", "compression()", StorageEnableCompression},
			{"/storage", StorageConfig, {
							{"path", StoragePath},
							{"metadata", StorageMetadata, '*'},
							{"acl", StorageEnableAcl, '?'},
							{"compression", StorageEnableCompression, '?'}
						}
			},
			{"/storage", "success", "context", bindings::method::Context::createStorage(), {{StorageConfig}} }
		}
	) {}
};

class SchemaDestroyStorage :public papuga::RequestAutomaton
{
public:
	SchemaDestroyStorage() :papuga::RequestAutomaton(
		getStrusClassDefs(), getBindingsInterfaceDescription()->structs,
		"storage",
		{
			{"/storage", "path()", StoragePath},
			{"/storage", "success", "context", bindings::method::Context::destroyStorage(), {{StorageConfig}} }
		}
	) {}
};

class SchemaOpenStorage :public papuga::RequestAutomaton
{
public:
	SchemaOpenStorage() :papuga::RequestAutomaton(
		getStrusClassDefs(), getBindingsInterfaceDescription()->structs,
		"storage",
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


