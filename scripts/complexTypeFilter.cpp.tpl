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
#include "structElementArray.hpp"
#include "stateTable.hpp"
#include "variantValueTemplate.hpp"
#include <cstring>

using namespace strus;

static const char* g_element_names[] = { {% for name in func("memberlist")(structname) %}"{{name}}",{% endfor %} 0};
static const filter::StructElementArray g_struct_elements( g_element_names);

enum TermState {
	StateEnd{% for state in func("statelist")( "State", structname) %},
	{{state}}{% endfor %}
};

enum TermArrayState {
	StateArrayEnd{% for state in func("statelist")( "State", structname + "[]") %},
	{{state}}{% endfor %}
};

#define _OPEN	BindingFilterInterface::Open
#define _CLOSE	BindingFilterInterface::Close
#define _INDEX	BindingFilterInterface::Index
#define _VALUE	BindingFilterInterface::Value
#define _NULL	filter::StateTable::NullValue
#define _TAG	filter::StateTable::TagValue
#define _ELEM	filter::StateTable::ElementValue


// Element: index, tag, nextState, skipState, valueType, tagnameIndex, valueIndex
static const filter::StateTable::Element g_struct_statetable[] = {
	{StateEnd, _CLOSE, StateEnd, StateEnd, _NULL, 0, 0}{% for statestruct in func("statestructlist")( "State", structname, "StateEnd") %},
	{{statestruct}}{% endfor %}
};

static const filter::StateTable::Element g_array_statetable[] = {
	{StateArrayEnd, _CLOSE, StateArrayEnd, StateArrayEnd, _NULL, 0, 0},
	{StateArrayIndex, _INDEX, StateArray{{func("uc1")(elements[0].name)}}Open, StateArrayIndex, _TAG, 1, 0}{% for statestruct in func("statestructlist")( "StateArray", structname, "StateArrayIndex") %},
	{{statestruct}}{% endfor %}
};

{{structname}}Filter::{{structname}}Filter()
	:m_impl(0),m_ownership(0),m_state(0)
{
	std::memset( m_index, 0, sizeof(m_index));
}

{{structname}}Filter::{{structname}}Filter( const {{structname}}Filter& o)
	:m_impl(o.m_impl),m_ownership(o.m_ownership),m_state(o.m_state)
{
	std::memcpy( m_index, o.m_index, sizeof(m_index));
}

{{structname}}Filter::{{structname}}Filter( const {{fullname}}* impl)
	:m_impl(impl),m_ownership(0),m_state(1)
{
	std::memset( m_index, 0, sizeof(m_index));
}

{{structname}}Filter::{{structname}}Filter( {{fullname}}* impl, bool withOwnership)
	:m_impl(impl),m_ownership(impl),m_state(1)
{
	std::memset( m_index, 0, sizeof(m_index));
}

{{structname}}Filter::~{{structname}}Filter()
{
	if (m_ownership) delete( m_ownership);
}

static bindings::ValueVariant getElementValue( const {{fullname}}& elem, int valueIndex)
{
	switch (valueIndex) {
{% for vaccess in func("valueaccesslist")( "(*m_impl)", structname) %}
		case {{loop.index0}}:
			return bindings::ValueVariant( {{vaccess}});
{% endfor %}
		}
	return bindings::ValueVariant();
}

BindingFilterInterface::Tag {{structname}}Filter::getNext( bindings::ValueVariant& val)
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
			val = g_struct_elements[ st.tagnameIndex];
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


