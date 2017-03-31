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
#ifndef _STRUS_BINDING_{{structname.upper()}}_FILTER_HPP_INCLUDED
#define _STRUS_BINDING_{{structname.upper()}}_FILTER_HPP_INCLUDED
/// \file {{structname[:1].lower() + structname[1:]}}Filter.hpp
#include "strus/bindingFilterInterface.hpp"
{% for incfile in includes %}#include {{incfile}}
{% endfor %}
#include <vector>

/// \brief strus toplevel namespace
namespace strus {

class {{structname}}Filter
	:public BindingFilterInterface
{
public:
	{{structname}}Filter();
	{{structname}}Filter( const {{structname}}Filter& o);
	explicit {{structname}}Filter( const {{fullname}}* impl);
	{{structname}}Filter( {{fullname}}* impl, bool withOwnership);

	virtual ~{{structname}}Filter();
	virtual Tag getNext( bindings::ValueVariant& val);

	virtual void skip();

	virtual BindingFilterInterface* createCopy() const;

private:
	enum {MaxDepth=8};

private:
	const {{fullname}}* m_impl;
	{{fullname}}* m_ownership;
	unsigned int m_state;
	unsigned int m_index[ MaxDepth];
};

}//namespace
#endif

