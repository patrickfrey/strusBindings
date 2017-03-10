/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_TEMPLATE_STRUCT_VECTOR_FILTER_HPP_INCLUDED
#define _STRUS_BINDING_TEMPLATE_STRUCT_VECTOR_FILTER_HPP_INCLUDED
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

/// \brief Iterator on the structure of a document created as result of a document analysis
template <class StructType, class StructTypeAccess>
class StructVectorFilter
	:public BindingFilterInterface
	,public filter::StructArrayStateMachine
{
public:
	/// \brief Default constructor
	StructVectorFilter()
		:filter::StructArrayStateMachine()
		,m_impl(0){}
	
	/// \brief Constructor
	StructVectorFilter( const std::vector<StructType>* impl, const filter::StructElementArray& elems)
		:filter::StructArrayStateMachine( elems, impl->size())
		,m_impl(impl){}
	
	/// \brief Copy constructor
	StructVectorFilter( const StructVectorFilter& o)
		:filter::StructArrayStateMachine(*this)
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
				val = StructTypeAccess::get( currentStateIndex(), (*m_impl)[ currentArrayIndex()]);
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
		return new (mem) StructVectorFilter( *this);
	}

private:
	const std::vector<StructType>* m_impl;
};

}//namespace
#endif

