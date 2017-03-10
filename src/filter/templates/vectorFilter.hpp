/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_VECTOR_FILTER_TEMPLATE_HPP_INCLUDED
#define _STRUS_BINDING_VECTOR_FILTER_TEMPLATE_HPP_INCLUDED
/// \brief Template for filters on vectors
/// \file vectorFilter.hpp
#include "strus/bindingFilterInterface.hpp"
#include "strus/analyzer/document.hpp"
#include "strus/reference.hpp"

/// \brief strus toplevel namespace
namespace strus {

/// \brief Iterator on the structure of an analyzer document created as result of a document analysis
template <class Element, class ElementFilter>
class VectorFilter
	:public BindingFilterInterface
{
public:
	/// \brief Default constructor
	VectorFilter()
		:m_state(0)
		,m_tag(BindingFilterInterface::Close)
		,m_impl(0)
		,m_elementFilter(){}

	explicit VectorFilter( const std::vector<Element>* impl)
		:m_state(0)
		,m_tag(BindingFilterInterface::Open)
		,m_impl(impl)
		,m_elementFilter()
	{
		if (impl->size() > 0)
		{
			m_elementFilter = ElementFilter( &(*impl)[0]);
		}
	}

	/// \brief Copy constructor
	VectorFilter( const AnalyzerDocumentFilter& o)
		:m_state(o.m_state)
		,m_tag(o.m_tag)
		,m_impl(o.m_impl)
		,m_elementFilter(o.m_elementFilter){}


	virtual Tag getNext( ValueVariant& val)
	{
		if (!m_impl || m_state >= m_impl->size()) return BindingFilterInterface::Close;
	
		Tag rt = m_tag;
		switch (m_tag)
		{
			case BindingFilterInterface::Close:
				m_state = (State)(m_state+1);
				m_tag = BindingFilterInterface::Open;
				break;
			case BindingFilterInterface::Open:
				m_tag = BindingFilterInterface::Value;
				break;
			case BindingFilterInterface::Value:
				m_tag = BindingFilterInterface::Close;
				val = getCurrentValue();
				break;
		}
		return rt;
	}

	virtual void skip();

	virtual BindingFilterInterface* createCopy( BindingFilterMem* mem) const;

private:
	const char* getCurrentTagName();

private:
	std::size_t m_state;
	BindingFilterInterface::Tag m_tag;
	Reference<analyzer::Document> m_impl;
	ElementFilter m_elementFilter;
};

}//namespace
#endif

