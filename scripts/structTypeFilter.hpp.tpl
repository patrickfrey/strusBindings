/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_{{structname.upper()}}_FILTER_HPP_INCLUDED
#define _STRUS_BINDING_{{structname.upper()}}_FILTER_HPP_INCLUDED
/// \file {{structname}}Filter.hpp
#include "strus/bindingFilterInterface.hpp"
{% for incfile in includes %}#include {{incfile}}
{% endfor %}

/// \brief strus toplevel namespace
namespace strus {

class {{structname.title()}}Filter
	:public BindingFilterInterface
{
public:
	{{structname.title()}}Filter();
	{{structname.title()}}Filter( const {{structname.title()}}Filter& o);
	explicit {{structname.title()}}Filter( const {{fullname}}* impl);
	{{structname.title()}}Filter( {{fullname}}* impl, bool withOwnership);

	virtual ~{{structname.title()}}Filter();
	virtual Tag getNext( binding::ValueVariant& val);

	virtual void skip();

	virtual BindingFilterInterface* createCopy() const;
	
private:
	const {{fullname}}* m_impl;
	{{fullname}}* m_ownership;
	unsigned int m_state;
};


class {{structname.title()}}VectorFilter
	:public BindingFilterInterface
{
public:
	{{structname.title()}}VectorFilter();
	{{structname.title()}}VectorFilter( const {{structname.title()}}VectorFilter& o);
	explicit {{structname.title()}}VectorFilter( const std::vector<{{fullname}}>* impl);
	{{structname.title()}}VectorFilter( std::vector<{{fullname}}>* impl, bool withOwnership);
	virtual ~{{structname.title()}}VectorFilter();

	virtual Tag getNext( binding::ValueVariant& val);

	virtual void skip();

	virtual BindingFilterInterface* createCopy() const;

private:
	const std::vector<{{fullname}}>* m_impl;
	std::vector<{{fullname}}>* m_ownership;
	unsigned int m_state;
	unsigned int m_index;
};

}//namespace
#endif

