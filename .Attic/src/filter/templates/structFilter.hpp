/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_TEMPLATE_STRUCT_FILTER_HPP_INCLUDED
#define _STRUS_BINDING_TEMPLATE_STRUCT_FILTER_HPP_INCLUDED
/// \brief Template for strus binding filters iterating on structures of atomic values
/// \file structFilter.hpp
#include "strus/bindingFilterInterface.hpp"
#include "stateMachines.hpp"
#include <vector>

/// \brief strus toplevel namespace
namespace strus {

/// Interface for StructTypeAccess
/// struct StructTypeAccess
/// {
///	static binding::ValueVariant get( unsigned int state, const StructType& st);
/// };

/// \brief Template for binding filters iterating on structures of atomic values
template <class StructType, class StructTypeAccess>
class StructFilter
	:public BindingFilterInterface
	,public filter::StructStateMachine
{
public:
	/// \brief Default constructor
	StructFilter()
		:filter::StructStateMachine()
		,m_impl(0){}

	/// \brief Constructor
	explicit StructFilter( const StructType* impl, const filter::StructElementArray& elems)
		:filter::StructStateMachine(elems)
		,m_impl(impl){}

	/// \brief Copy constructor
	StructFilter( const StructFilter& o)
		:filter::StructStateMachine(*this)
		,m_impl(o.m_impl){}

	virtual Tag getNext( binding::ValueVariant& val)
	{
		BindingFilterInterface::Tag rt = currentTag();
		switch (rt)
		{
			case BindingFilterInterface::Close:
			case BindingFilterInterface::Index:
				val.clear();
				break;
			case BindingFilterInterface::Open:
				val = currentTagValue();
				break;
			case BindingFilterInterface::Value:
				val = StructTypeAccess::get( currentStateIndex(), *m_impl);
				break;
		}
		skipNext();
		return rt;
	}

	virtual void skip()
	{
		skipScope();
	}

	virtual BindingFilterInterface* createCopy( BindingFilterMem* mem) const
	{
		return new (mem) StructFilter( *this);
	}

private:
	const StructType* m_impl;
};

}//namespace
#endif

