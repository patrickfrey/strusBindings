/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Interface for introspection of a static structure defined as StructView
#ifndef _STRUS_BINDING_IMPL_VALUE_STRUCTVIEW_INTROSPECTION_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_VALUE_STRUCTVIEW_INTROSPECTION_HPP_INCLUDED
#include "introspectionBase.hpp"
#include "strus/structView.hpp"
#include "strus/errorBufferInterface.hpp"

namespace strus {
namespace bindings {

class StructViewIntrospection
	:public IntrospectionBase
{
public:
	StructViewIntrospection(
			ErrorBufferInterface* errorhnd_,
			const StructView& impl_)
		:m_errorhnd(errorhnd_)
		,m_impl(impl_)
		{}
	virtual ~StructViewIntrospection(){}

	virtual void serialize( papuga_Serialization& serialization, bool substructure);
	virtual IntrospectionBase* open( const std::string& name);
	virtual std::vector<IntrospectionLink> list();

private:
	ErrorBufferInterface* m_errorhnd;
	StructView m_impl;
};

}}//namespace
#endif
