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

class SchemaVectorStoragePart :public AutomatonNameSpace
{
public:
	static papuga::RequestAutomaton_NodeList defineVectorStorage( const char* rootexpr)
	{
		return papuga::RequestAutomaton_NodeList( rootexpr,
		{
			{"path", "()", DatabasePath, papuga_TypeString, "strus/vstorage"},
			{"memtypes", "()", VectorMemType, papuga_TypeString, "V;A"},
			{"lexprun", "()", VectorLexemPrunning, papuga_TypeBool, "1;2;4;5"},
			{"vecdim", "()", VectorDim, papuga_TypeInt, "256;300;500"},
			{"bits", "()", VectorBits, papuga_TypeInt, "64"},
			{"variations", "()", VectorVariations, papuga_TypeInt, "32"},
			{"cache", "()", DatabaseLruCacheSize, papuga_TypeString, "200M"},
			{"autocompact", "()", DatabaseEnableAutoCompact, papuga_TypeBool, "true"},
			{"compression", "()", DatabaseEnableCompression, papuga_TypeBool, "true"},
			{"max_open_files", "()", DatabaseMaxNofOpenFiles, papuga_TypeInt, "128"},
			{"write_buffer_size", "()", DatabaseWriteBufferSize, papuga_TypeString, "4M"},
			{"block_size", "()", DatabaseBlockSize, papuga_TypeString, "4K"},
			{"", StorageConfig, {
					{"path", DatabasePath},
					{"memtypes", VectorMemType, '*'},
					{"lexprun", VectorLexemPrunning, '?'},
					{"vecdim", VectorDim, '?'},
					{"bits", VectorBits, '?'},
					{"variations", VectorVariations, '?'},
					{"cache", DatabaseLruCacheSize, '?'},
					{"autocompact", DatabaseEnableAutoCompact, '?'},
					{"compression", DatabaseEnableCompression, '?'},
					{"max_open_files", DatabaseMaxNofOpenFiles, '?'},
					{"write_buffer_size", DatabaseWriteBufferSize, '?'},
					{"block_size", DatabaseBlockSize, '?'},
				}
			},
		});
	}
};

class Schema_Context_POST_VectorStorage :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_Context_POST_VectorStorage() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, true/*strict*/,
		{},
		{},
		{
			{SchemaVectorStoragePart::defineVectorStorage( "/vstorage")},
			{"/vstorage", "_success", "context", bindings::method::Context::createVectorStorage(), {{StorageConfig}} },
			{"/vstorage", "storage", "context", bindings::method::Context::createVectorStorageClient(), {{StorageConfig}} }
		}
	) {}
};

class Schema_Context_DELETE_POST_VectorStorage :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_Context_DELETE_POST_VectorStorage() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, true/*strict*/,
		{},
		{},
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

class Schema_Context_PUT_VectorStorage :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_Context_PUT_VectorStorage() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, true/*strict*/,
		{},
		{},
		{
			{SchemaVectorStoragePart::defineVectorStorage( "/vstorage")},
			{"/", "vstorage", "context", bindings::method::Context::createVectorStorageClient(), {{StorageConfig}} }
		}
	) {}
};

class Schema_Context_PUT_VectorStorageTransaction :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_Context_PUT_VectorStorageTransaction() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, true/*strict*/,
		{},
		{},
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

