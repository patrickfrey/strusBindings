/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/*
 * DO NOT MODIFY THIS FILE !!!
 * 
 * This file has been generated with the script scripts/genFilters.py
 * Modifications on this file will be lost!
 */
#ifndef _STRUS_BINDING_ATTRIBUTE_FILTER_HPP_INCLUDED
#define _STRUS_BINDING_ATTRIBUTE_FILTER_HPP_INCLUDED
/// \file attributeFilter.hpp
#include "strus/bindingFilterInterface.hpp"
#include "strus/analyzer/attribute.hpp"


/// \brief strus toplevel namespace
namespace strus {

class AttributeFilter
	:public BindingFilterInterface
{
public:
	AttributeFilter();
	AttributeFilter( const AttributeFilter& o);
	explicit AttributeFilter( const analyzer::Attribute* impl);
	AttributeFilter( analyzer::Attribute* impl, bool withOwnership);

	virtual ~AttributeFilter();
	virtual Tag getNext( binding::ValueVariant& val);

	virtual void skip();

	virtual BindingFilterInterface* createCopy() const;
	
private:
	const analyzer::Attribute* m_impl;
	analyzer::Attribute* m_ownership;
	unsigned int m_state;
};


class AttributeVectorFilter
	:public BindingFilterInterface
{
public:
	AttributeVectorFilter();
	AttributeVectorFilter( const AttributeVectorFilter& o);
	explicit AttributeVectorFilter( const std::vector<analyzer::Attribute>* impl);
	AttributeVectorFilter( std::vector<analyzer::Attribute>* impl, bool withOwnership);
	virtual ~AttributeVectorFilter();

	virtual Tag getNext( binding::ValueVariant& val);

	virtual void skip();

	virtual BindingFilterInterface* createCopy() const;

private:
	const std::vector<analyzer::Attribute>* m_impl;
	std::vector<analyzer::Attribute>* m_ownership;
	unsigned int m_state;
	unsigned int m_index;
};

}//namespace
#endif
