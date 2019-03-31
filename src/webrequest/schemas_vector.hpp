/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* \brief Schemas for vectors
 * @file schemas_vector.hpp
 */
#ifndef _STRUS_WEBREQUEST_SCHEMAS_VECTOR_HPP_INCLUDED
#define _STRUS_WEBREQUEST_SCHEMAS_VECTOR_HPP_INCLUDED
#include "schemas_base.hpp"

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class Schema_Context_CREATE_VectorStorage :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_Context_CREATE_VectorStorage() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		NULL/*resultname*/,{},
		{
			{"/vstorage/path", "()", DatabasePath, papuga_TypeString, "strus/vstorage"},
			{"/vstorage/vecdim", "()", VectorDim, papuga_TypeInt, "256;300;500"},
			{"/vstorage/bits", "()", VectorBits, papuga_TypeInt, "64"},
			{"/vstorage/variations", "()", VectorVariations, papuga_TypeInt, "32"},
			{"/vstorage/compression", "()", DatabaseEnableCompression, papuga_TypeBool, "true"},
			{"/vstorage/cache", "()", DatabaseLruCacheSize, papuga_TypeString, "200M"},
			{"/vstorage/max_open_files", "()", DatabaseMaxNofOpenFiles, papuga_TypeInt, "128"},
			{"/vstorage/write_buffer_size", "()", DatabaseWriteBufferSize, papuga_TypeString, "4M"},
			{"/vstorage/block_size", "()", DatabaseBlockSize, papuga_TypeString, "4K"},
			{"/vstorage", StorageConfig, {
					{"path", DatabasePath},
					{"vecdim", VectorDim, '!'},
					{"bits", VectorBits, '?'},
					{"variations", VectorVariations, '?'},
					{"compression", DatabaseEnableCompression, '?'},
					{"cache", DatabaseLruCacheSize, '?'},
					{"max_open_files", DatabaseMaxNofOpenFiles, '?'},
					{"write_buffer_size", DatabaseWriteBufferSize, '?'},
					{"block_size", DatabaseBlockSize, '?'},
				}
			},
			{"/vstorage", "_success", "context", bindings::method::Context::createVectorStorage(), {{StorageConfig}} },
			{"/vstorage", "storage", "context", bindings::method::Context::createVectorStorageClient(), {{StorageConfig}} }
		}
	) {}
};

class Schema_Context_DELETE_VectorStorage :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_Context_DELETE_VectorStorage() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		NULL/*resultname*/,{},
		{
			{"/vstorage/path", "()", DatabasePath, papuga_TypeString, "strus/vstorage"},
			{"/vstorage", StorageConfig, {
					{"path", DatabasePath}
				}
			},
			{"/vstorage", "success", "context", bindings::method::Context::destroyStorage(), {{StorageConfig}} }
		}
	) {}
};

class Schema_Context_INIT_VectorStorage :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_Context_INIT_VectorStorage() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		NULL/*resultname*/,{},
		{
			{"/vstorage/path", "()", DatabasePath, papuga_TypeString, "strus/vstorage"},
			{"/vstorage/memtypes", "()", VectorMemType, papuga_TypeString, "V;A"},
			{"/vstorage/lexprun", "()", VectorLexemPrunning, papuga_TypeBool, "1;2;4;5"},
			{"/vstorage/cache", "()", DatabaseLruCacheSize, papuga_TypeString, "200M"},
			{"/vstorage/autocompact", "()", DatabaseEnableAutoCompact, papuga_TypeBool, "true"},
			{"/vstorage/compression", "()", DatabaseEnableCompression, papuga_TypeBool, "true"},
			{"/vstorage/max_open_files", "()", DatabaseMaxNofOpenFiles, papuga_TypeInt, "128"},
			{"/vstorage/write_buffer_size", "()", DatabaseWriteBufferSize, papuga_TypeString, "4M"},
			{"/vstorage/block_size", "()", DatabaseBlockSize, papuga_TypeString, "4K"},
			{"/vstorage", StorageConfig, {
					{"path", DatabasePath},
					{"memtypes", VectorMemType, '*'},
					{"lexprun", VectorLexemPrunning, '?'},
					{"cache", DatabaseLruCacheSize, '?'},
					{"autocompact", DatabaseEnableAutoCompact, '?'},
					{"compression", DatabaseEnableCompression, '?'},
					{"max_open_files", DatabaseMaxNofOpenFiles, '?'},
					{"write_buffer_size", DatabaseWriteBufferSize, '?'},
					{"block_size", DatabaseBlockSize, '?'}
				}
			},
			{"/", "vstorage", "context", bindings::method::Context::createVectorStorageClient(), {{StorageConfig}} }
		}
	) {}
};

class Schema_Context_PUT_VectorStorageTransaction :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_Context_PUT_VectorStorageTransaction() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs,
		NULL/*resultname*/,{},
		{
			{"/vstorage/feature/name", "()", VectorFeatureName, papuga_TypeString, "main_station"},
			{"/vstorage/feature/type", "()", VectorFeatureType, papuga_TypeString, "N;E"},
			{"/vstorage/feature/vector", "()", VectorFeatureVector, papuga_TypeDouble, "0.1223;0.09823;0.10948;0.09883"},
			{"/vstorage/feature", VectorFeature, {
						{"name", VectorFeatureName, '!'},
						{"type", VectorFeatureType, '!'},
						{"vector", VectorFeatureVector, '*'}
					}
			},
			{"/vstorage/feature", "", "transaction", bindings::method::VectorStorageTransaction::defineVector(), {{VectorFeatureType,'!'},{VectorFeatureName,'!'},{VectorFeatureVector,'*'}} },
			{"/vstorage", "", "transaction", bindings::method::VectorStorageTransaction::commit(), {}}
		}
	) {}
};

}}//namespace
#endif

