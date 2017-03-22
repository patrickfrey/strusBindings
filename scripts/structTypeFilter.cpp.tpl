/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \file {{structname}}Filter.cpp
#include "{{structname}}Filter.hpp"
#include "filter/structElementArray.hpp"

using namespace strus;

static const char* g_element_names[] = {% for element in elements %}"{{element.name}}",{% endfor %}0};
static const filter::StructElementArray g_struct_elements( g_element_names);

enum TermState {
	StateEnd{% for element in elements %},
	State{{element.name.title()}}Open,
	State{{element.name.title()}}Value,
	State{{element.name.title()}}Close{% endfor %}}
};

enum TermArrayState {
	StateArrayEnd,
	StateArrayIndex{% for element in elements %},
	StateArray{{element.name.title()}}Open,
	StateArray{{element.name.title()}}Value,
	StateArray{{element.name.title()}}Close{% endfor %}
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
	{State{{element.name.title()}}Open,	_OPEN,  State{{element.name.title()}}Value,		State{% if loop.index0 +1 == elements.len() %}StateEnd{% else %}{{elements[loop.index0+1].name.title()}}Open{% endif %},	_TAG,	 0, {{loop.index0}}},
	{State{{element.name.title()}}Value,	_VALUE, State{{element.name.title()}}Close,		State{% if loop.index0 +1 == elements.len() %}StateEnd{% else %}{{elements[loop.index0+1].name.title()}}Open{% endif %},	_ELEM,	 0, {{loop.index0}}},
	{State{{element.name.title()}}Close,	_CLOSE, State{% if loop.index0 +1 == elements.len() %}StateEnd{% else %}{{elements[loop.index0+1].name.title()}}Open{% endif %},	State{% if loop.index0 +1 == elements.len() %}StateEnd{% else %}{{elements[loop.index0+1].name.title()}}Open{% endif %},	_NULL,	 0, {{loop.index0}}},
	{% endfor %}
};

static const filter::StateTable::Element g_array_statetable[] = {
	{StateArrayEnd,		_CLOSE, StateArrayEnd,		StateArrayEnd,		_NULL,	 0, 0},
	{StateArrayIndex,	_INDEX, StateArray{{elements[0].name.title()}}Open,	StateArrayIndex,	_TAG,	 1, 0},
	{% for element in elements %}
	{StateArray{{element.name.title()}}Open,	_OPEN,  StateArray{{element.name.title()}}Value,		StateArray{% if loop.index0 +1 == elements.len() %}StateArrayEnd{% else %}{{elements[loop.index0+1].name.title()}}Open{% endif %},	_TAG,	 0, {{loop.index0}}},
	{StateArray{{element.name.title()}}Value,	_VALUE, StateArray{{element.name.title()}}Close,		StateArray{% if loop.index0 +1 == elements.len() %}StateArrayEnd{% else %}{{elements[loop.index0+1].name.title()}}Open{% endif %},	_ELEM,	 0, {{loop.index0}}},
	{StateArray{{element.name.title()}}Close,	_CLOSE, StateArray{% if loop.index0 +1 == elements.len() %}StateArrayEnd{% else %}{{elements[loop.index0+1].name.title()}}Open{% endif %},	State{% if loop.index0 +1 == elements.len() %}StateEnd{% else %}{{elements[loop.index0+1].name.title()}}Open{% endif %},	_NULL,	 0, {{loop.index0}}},
	{% endfor %}
};

{{structname.title()}}Filter::{{structname.title()}}Filter()
	:m_impl(0),m_ownership(0),m_state(0){}

{{structname.title()}}Filter::{{structname.title()}}Filter( const {{structname.title()}}Filter& o)
	:m_impl(o.m_impl),m_ownership(o.m_ownership),m_state(o.m_state){}

{{structname.title()}}Filter::{{structname.title()}}Filter( const {{fullname}}* impl)
	:m_impl(impl),m_ownership(0),m_state(1){}

{{structname.title()}}Filter::{{structname.title()}}Filter( {{fullname}}* impl, bool withOwnership)
	:m_impl(impl),m_ownership(impl),m_state(1){}

{{structname.title()}}Filter::~{{structname.title()}}Filter()
{
	if (m_ownership) delete( m_ownership);
}

static binding::ValueVariant getElementValue( const {{fullname}}& elem, int valueIndex)
{
	switch (valueIndex)
	{{% for element in elements %}{% if elem.type == "string" %}
		case {{loop.index0}}:
			return binding::ValueVariant( elem.{{element.name}}().c_str(), elem.{{element.name}}().size());
		{% endif %}{% if elem.type == "charp" %}
			return binding::ValueVariant( elem.{{element.name}}());
		{% endif %}{% if elem.type == "uint" %}
			return binding::ValueVariant( (binding::ValueVariant::UIntType)elem.{{element.name}}());
		{% endif %}{% if elem.type == "int" %}
			return binding::ValueVariant( (binding::ValueVariant::IntType)elem.{{element.name}}());
		{% endif %}{% if elem.type == "double" %}
			return binding::ValueVariant( (double)elem.{{element.name}}());
		{% endif %}{% if elem.type == "NumericVariant" %}
			return binding::ValueVariant( elem.{{element.name}}());
		{% endfor %}
	}
	return binding::ValueVariant();
}

BindingFilterInterface::Tag {{structname.title()}}Filter::getNext( binding::ValueVariant& val)
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

void {{structname.title()}}Filter::skip()
{
	const filter::StateTable::Element& st = g_struct_statetable[ m_state];
	m_state = st.skipState;
}

BindingFilterInterface* {{structname.title()}}Filter::createCopy() const
{
	return new {{structname.title()}}Filter(*this);
}

{{structname.title()}}VectorFilter::{{structname.title()}}VectorFilter()
	:m_impl(0),m_ownership(0),m_state(0),m_index(0){}

{{structname.title()}}VectorFilter::{{structname.title()}}VectorFilter( const {{structname.title()}}VectorFilter& o)
	:m_impl(o.m_impl),m_ownership(o.m_ownership),m_state(o.m_state),m_index(o.m_index){}

{{structname.title()}}VectorFilter::{{structname.title()}}VectorFilter( const std::vector<{{fullname}}>* impl)
	:m_impl(impl),m_ownership(0),m_state(1),m_index(0){}

{{structname.title()}}VectorFilter::{{structname.title()}}VectorFilter( std::vector<{{fullname}}>* impl, bool withOwnership)
	:m_impl(impl),m_ownership(impl),m_state(1),m_index(0){}


BindingFilterInterface::Tag {{structname.title()}}VectorFilter::getNext( binding::ValueVariant& val)
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

void {{structname.title()}}VectorFilter::skip()
{
	const filter::StateTable::Element& st = g_array_statetable[ m_state];
	m_state = st.skipState;
	if (m_state == StateArrayIndex && m_impl && m_index < m_impl->size())
	{
		++m_index;
	}
}

BindingFilterInterface* {{structname.title()}}VectorFilter::createCopy() const
{
	return new {{structname.title()}}VectorFilter(*this);
}

