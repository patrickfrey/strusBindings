/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Interface for introspection of a vector storage client
#ifndef _STRUS_BINDING_IMPL_VALUE_VECTOR_STORAGE_INTROSPECTION_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_VALUE_VECTOR_STORAGE_INTROSPECTION_HPP_INCLUDED
#include "introspectionBase.hpp"
#include "strus/vectorStorageClientInterface.hpp"
#include "strus/errorBufferInterface.hpp"

namespace strus {
namespace bindings {

class VectorStorageIntrospection
	:public IntrospectionBase
{
public:
	VectorStorageIntrospection(
			ErrorBufferInterface* errorhnd_,
			const VectorStorageClientInterface* impl_)
		:m_errorhnd(errorhnd_)
		,m_impl(impl_)
		{}
	virtual ~VectorStorageIntrospection(){}

	virtual void serialize( papuga_Serialization& serialization, bool substructure);
	virtual IntrospectionBase* open( const std::string& name);
	virtual std::vector<IntrospectionLink> list();

private:
	ErrorBufferInterface* m_errorhnd;
	const VectorStorageClientInterface* m_impl;
};

}}//namespace
#endif

