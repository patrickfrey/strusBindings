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
#include "schemas_vector_decl.hpp"

#if __cplusplus < 201103L
#error Need C++11 or later to include this
#endif

namespace strus {
namespace webrequest {

class Schema_Context_POST_VectorStorage :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_Context_POST_VectorStorage() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, itemName, true/*strict*/, false/*exclusive*/,
		{/*env*/{"path", EnvFormat, "storage/{id}/{name}"}},
		{/*result*/},
		{/*inherit*/},
		{/*input*/
			{SchemaVectorStoragePart::defineVectorStorage( "/vstorage")},
			{"/vstorage", 0, "context", bindings::method::Context::createVectorStorage(), {{StorageConfig}} },
			{"/vstorage", "vstorage", "context", bindings::method::Context::createVectorStorageClient(), {{StorageConfig}} }
		}
	) {}
};

class Schema_Context_DELETE_POST_VectorStorage :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_Context_DELETE_POST_VectorStorage() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, itemName, true/*strict*/, false/*exclusive*/,
		{/*env*/{"path", EnvFormat, "storage/{id}/{name}"}},
		{/*result*/},
		{/*inherit*/},
		{/*input*/
			{"/vstorage/path", "()", DatabasePath, papuga_TypeString, "strus/vstorage"},
			{"/vstorage", StorageConfig, {
					{"path", DatabasePath}
				}
			},
			{"/vstorage", 0, "context", bindings::method::Context::destroyStorage(), {{StorageConfig}} }
		}
	) {}
};

class Schema_Context_PUT_VectorStorage :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_Context_PUT_VectorStorage() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, itemName, true/*strict*/, false/*exclusive*/,
		{/*env*/},
		{/*result*/},
		{/*inherit*/},
		{/*input*/
			{SchemaVectorStoragePart::defineVectorStorage( "/vstorage")},
			{"/", "vstorage", "context", bindings::method::Context::createVectorStorageClient(), {{StorageConfig}} }
		}
	) {}
};

class Schema_Context_PUT_VectorStorageTransaction :public papuga::RequestAutomaton, public AutomatonNameSpace
{
public:
	Schema_Context_PUT_VectorStorageTransaction() :papuga::RequestAutomaton(
		strus_getBindingsClassDefs(), getBindingsInterfaceDescription()->structs, itemName, true/*strict*/, false/*exclusive*/,
		{/*env*/},
		{/*result*/},
		{/*inherit*/},
		{/*input*/
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

