/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Interface for introspection of a storage client
#ifndef _STRUS_BINDING_IMPL_VALUE_STORAGE_INTROSPECTION_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_VALUE_STORAGE_INTROSPECTION_HPP_INCLUDED
#include "introspectionBase.hpp"
#include "strus/storageClientInterface.hpp"
#include "strus/errorBufferInterface.hpp"

namespace strus {
namespace bindings {

class StorageIntrospection
	:public IntrospectionBase
{
public:
	StorageIntrospection(
			ErrorBufferInterface* errorhnd_,
			const StorageClientInterface* impl_)
		:m_errorhnd(errorhnd_)
		,m_impl(impl_)
		{}
	virtual ~StorageIntrospection(){}

	virtual void serialize( papuga_Serialization& serialization, const std::string& path, bool substructure);
	virtual IntrospectionBase* open( const std::string& name);
	virtual std::vector<IntrospectionLink> list();

private:
	ErrorBufferInterface* m_errorhnd;
	const StorageClientInterface* m_impl;
};

}}//namespace
#endif

