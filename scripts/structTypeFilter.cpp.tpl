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
/// \file {{structname[:1].lower() + structname[1:]}}Filter.cpp
#include "{{structname[:1].lower() + structname[1:]}}Filter.hpp"
#include "filter/structElementArray.hpp"

using namespace strus;

static const char* g_element_names[] = {% for element in elements %}"{{element.name}}", {% endfor %}0};
static const filter::StructElementArray g_struct_elements( g_element_names);

enum TermState {
	StateEnd{% for element in elements %},
	State{{element.name[:1].upper() + element.name[1:]}}Open,
	State{{element.name[:1].upper() + element.name[1:]}}Value,
	State{{element.name[:1].upper() + element.name[1:]}}Close{% endfor %}
};

enum TermArrayState {
	StateArrayEnd,
	StateArrayIndex{% for element in elements %},
	StateArray{{element.name[:1].upper() + element.name[1:]}}Open,
	StateArray{{element.name[:1].upper() + element.name[1:]}}Value,
	StateArray{{element.name[:1].upper() + element.name[1:]}}Close{% endfor %}
};

#define _OPEN	BindingFilterInterface::Open
#define _CLOSE	BindingFilterInterface::Close
#define _INDEX	BindingFilterInterface::Index
#define _VALUE	BindingFilterInterface::Value
#define _NULL	filter::StateTable::NullValue
#define _TAG	filter::StateTable::TagValue
#define _ELEM	filter::StateTable::ElementValue


// Element: index, tag, nextState, skipState, valueType, tableIndex, valueIndex
static const filter::StateTable::Element g_struct_statetable[] = {
	{StateEnd,		_CLOSE, StateEnd,		StateEnd,		_NULL,	 0, 0},
	{% for element in elements %}
	{State{{element.name[:1].upper() + element.name[1:]}}Open,	_OPEN,  State{{element.name[:1].upper() + element.name[1:]}}Value,		State{% if loop.index0 +1 == elements |length %}End{% else %}{{elements[loop.index0+1].name.title()}}Open{% endif %},	_TAG,	 0, {{loop.index0}}},
	{State{{element.name[:1].upper() + element.name[1:]}}Value,	_VALUE, State{{element.name[:1].upper() + element.name[1:]}}Close,		State{% if loop.index0 +1 == elements |length %}End{% else %}{{elements[loop.index0+1].name.title()}}Open{% endif %},	_ELEM,	 0, {{loop.index0}}},
	{State{{element.name[:1].upper() + element.name[1:]}}Close,	_CLOSE, State{% if loop.index0 +1 == elements |length %}End{% else %}{{elements[loop.index0+1].name.title()}}Open{% endif %},	State{% if loop.index0 +1 == elements |length %}End{% else %}{{elements[loop.index0+1].name.title()}}Open{% endif %},	_NULL,	 0, {{loop.index0}}},
	{% endfor %}
};

static const filter::StateTable::Element g_array_statetable[] = {
	{StateArrayEnd,		_CLOSE, StateArrayEnd,		StateArrayEnd,		_NULL,	 0, 0},
	{StateArrayIndex,	_INDEX, StateArray{{elements[0].name.title()}}Open,	StateArrayIndex,	_TAG,	 1, 0},
	{% for element in elements %}
	{StateArray{{element.name[:1].upper() + element.name[1:]}}Open,	_OPEN,  StateArray{{element.name[:1].upper() + element.name[1:]}}Value,		StateArray{% if loop.index0 +1 == elements |length %}Index{% else %}{{elements[loop.index0+1].name.title()}}Open{% endif %},	_TAG,	 0, {{loop.index0}}},
	{StateArray{{element.name[:1].upper() + element.name[1:]}}Value,	_VALUE, StateArray{{element.name[:1].upper() + element.name[1:]}}Close,		StateArray{% if loop.index0 +1 == elements |length %}Index{% else %}{{elements[loop.index0+1].name.title()}}Open{% endif %},	_ELEM,	 0, {{loop.index0}}},
	{StateArray{{element.name[:1].upper() + element.name[1:]}}Close,	_CLOSE, StateArray{% if loop.index0 +1 == elements |length %}Index{% else %}{{elements[loop.index0+1].name.title()}}Open{% endif %},	StateArray{% if loop.index0 +1 == elements |length %}Index{% else %}{{elements[loop.index0+1].name.title()}}Open{% endif %},	_NULL,	 0, {{loop.index0}}},
	{% endfor %}
};

{{structname}}Filter::{{structname}}Filter()
	:m_impl(0),m_ownership(0),m_state(0){}

{{structname}}Filter::{{structname}}Filter( const {{structname}}Filter& o)
	:m_impl(o.m_impl),m_ownership(o.m_ownership),m_state(o.m_state){}

{{structname}}Filter::{{structname}}Filter( const {{fullname}}* impl)
	:m_impl(impl),m_ownership(0),m_state(1){}

{{structname}}Filter::{{structname}}Filter( {{fullname}}* impl, bool withOwnership)
	:m_impl(impl),m_ownership(impl),m_state(1){}

{{structname}}Filter::~{{structname}}Filter()
{
	if (m_ownership) delete( m_ownership);
}

static binding::ValueVariant getElementValue( const {{fullname}}& elem, int valueIndex)
{
	switch (valueIndex) {
	{% for element in elements %}
		case {{loop.index0}}:{% if element.type == "string" %}
			return binding::ValueVariant( elem.{{element.name}}().c_str(), elem.{{element.name}}().size());
		{% endif %}{% if element.type == "charp" %}
			return binding::ValueVariant( (const char*)elem.{{element.name}}());
		{% endif %}{% if element.type == "uint" %}
			return binding::ValueVariant( (binding::ValueVariant::UIntType)elem.{{element.name}}());
		{% endif %}{% if element.type == "int" %}
			return binding::ValueVariant( (binding::ValueVariant::IntType)elem.{{element.name}}());
		{% endif %}{% if element.type == "double" %}
			return binding::ValueVariant( (double)elem.{{element.name}}());
		{% endif %}{% if element.type == "NumericVariant" %}
			return binding::ValueVariant( elem.{{element.name}}());
		{% endif %}{% endfor %}
	}
	return binding::ValueVariant();
}

BindingFilterInterface::Tag {{structname}}Filter::getNext( binding::ValueVariant& val)
{
	const filter::StateTable::Element& st = g_struct_statetable[ m_state];
	Tag rt = st.tag;
	if (!m_impl)
	{
		val.clear();
		return rt;
	}
	switch (st.valueType)
	{
		case _NULL:
			val.clear();
			break;
		case _TAG:
			val = g_struct_elements[ st.valueIndex];
			break;
		case _ELEM:
			val = getElementValue( *m_impl, st.valueIndex);
			break;
	}
	m_state = st.nextState;
	return rt;
}

void {{structname}}Filter::skip()
{
	const filter::StateTable::Element& st = g_struct_statetable[ m_state];
	m_state = st.skipState;
}

BindingFilterInterface* {{structname}}Filter::createCopy() const
{
	return new {{structname}}Filter(*this);
}

{{structname}}VectorFilter::{{structname}}VectorFilter()
	:m_impl(0),m_ownership(0),m_state(0),m_index(0){}

{{structname}}VectorFilter::{{structname}}VectorFilter( const {{structname}}VectorFilter& o)
	:m_impl(o.m_impl),m_ownership(o.m_ownership),m_state(o.m_state),m_index(o.m_index){}

{{structname}}VectorFilter::{{structname}}VectorFilter( const std::vector<{{fullname}}>* impl)
	:m_impl(impl),m_ownership(0),m_state(1),m_index(0){}

{{structname}}VectorFilter::{{structname}}VectorFilter( std::vector<{{fullname}}>* impl, bool withOwnership)
	:m_impl(impl),m_ownership(impl),m_state(1),m_index(0){}


BindingFilterInterface::Tag {{structname}}VectorFilter::getNext( binding::ValueVariant& val)
{
	const filter::StateTable::Element& st = g_array_statetable[ m_state];
	Tag rt = st.tag;
	if (!m_impl || m_index >= m_impl->size())
	{
		val.clear();
		return rt;
	}

	switch (st.valueType)
	{
		case _NULL:
			val.clear();
			break;
		case _TAG:
			val = g_struct_elements[ st.valueIndex];
			break;
		case _ELEM:
			val = getElementValue( (*m_impl)[ m_index], st.valueIndex);
			break;
	}
	m_state = st.nextState;
	if (m_state == StateArrayIndex && m_impl && m_index < m_impl->size())
	{
		m_state = 1;
		++m_index;
	}
	return rt;
}

void {{structname}}VectorFilter::skip()
{
	const filter::StateTable::Element& st = g_array_statetable[ m_state];
	m_state = st.skipState;
	if (m_state == StateArrayIndex && m_impl && m_index < m_impl->size())
	{
		++m_index;
	}
}

BindingFilterInterface* {{structname}}VectorFilter::createCopy() const
{
	return new {{structname}}VectorFilter(*this);
}

