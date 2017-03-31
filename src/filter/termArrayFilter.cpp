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
/// \file termArrayFilter.cpp
#include "termArrayFilter.hpp"
#include "structElementArray.hpp"
#include "stateTable.hpp"
#include <cstring>

using namespace strus;

static const char* g_element_names[] = { "type","value","pos","len", 0};
static const filter::StructElementArray g_struct_elements( g_element_names);

enum TermState {
	StateEnd,
	StateIndex,
	StateTypeOpen,
	StateTypeValue,
	StateTypeClose,
	StateValueOpen,
	StateValueValue,
	StateValueClose,
	StatePosOpen,
	StatePosValue,
	StatePosClose,
	StateLenOpen,
	StateLenValue,
	StateLenClose
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
	{StateEnd, _CLOSE, StateEnd, StateEnd, _NULL, 0, 0},
	{StateIndex, _INDEX, StateTypeOpen, StateIndex, _TAG, -1, -1},
	{StateTypeOpen, _OPEN, StateTypeValue, StateValueOpen, _TAG, 0, -1},
	{StateTypeValue, _VALUE, StateTypeClose, StateTypeClose, _ELEM, -1, 0},
	{StateTypeClose, _CLOSE, StateValueOpen, StateValueOpen, _NULL, -1, -1},
	{StateValueOpen, _OPEN, StateValueValue, StatePosOpen, _TAG, 1, -1},
	{StateValueValue, _VALUE, StateValueClose, StateValueClose, _ELEM, -1, 1},
	{StateValueClose, _CLOSE, StatePosOpen, StatePosOpen, _NULL, -1, -1},
	{StatePosOpen, _OPEN, StatePosValue, StateLenOpen, _TAG, 2, -1},
	{StatePosValue, _VALUE, StatePosClose, StatePosClose, _ELEM, -1, 2},
	{StatePosClose, _CLOSE, StateLenOpen, StateLenOpen, _NULL, -1, -1},
	{StateLenOpen, _OPEN, StateLenValue, StateIndex, _TAG, 3, -1},
	{StateLenValue, _VALUE, StateLenClose, StateLenClose, _ELEM, -1, 3},
	{StateLenClose, _CLOSE, StateIndex, StateIndex, _NULL, -1, -1}
};

TermArrayFilter::TermArrayFilter()
	:m_impl(0),m_ownership(0),m_state(0)
{
	std::memset( m_index, 0, sizeof(m_index));
}

TermArrayFilter::TermArrayFilter( const TermArrayFilter& o)
	:m_impl(o.m_impl),m_ownership(o.m_ownership),m_state(o.m_state)
{
	std::memcpy( m_index, o.m_index, sizeof(m_index));
}

TermArrayFilter::TermArrayFilter( const std::vector<analyzer::Term>* impl)
	:m_impl(impl),m_ownership(0),m_state(1)
{
	std::memset( m_index, 0, sizeof(m_index));
}

TermArrayFilter::TermArrayFilter( std::vector<analyzer::Term>* impl, bool withOwnership)
	:m_impl(impl),m_ownership(impl),m_state(1)
{
	std::memset( m_index, 0, sizeof(m_index));
}

TermArrayFilter::~TermArrayFilter()
{
	if (m_ownership) delete( m_ownership);
}

static bindings::ValueVariant getElementValue( const std::vector<analyzer::Term>& elem, int valueIndex)
{
	switch (valueIndex) {

		case 0:
			return bindings::ValueVariant( (*m_impl)[m_index[0]].type());

		case 1:
			return bindings::ValueVariant( (*m_impl)[m_index[0]].value());

		case 2:
			return bindings::ValueVariant( (bindings::ValueVariant::UIntType)(*m_impl)[m_index[0]].pos());

		case 3:
			return bindings::ValueVariant( (bindings::ValueVariant::UIntType)(*m_impl)[m_index[0]].len());

	}
	return bindings::ValueVariant();
}

BindingFilterInterface::Tag TermArrayFilter::getNext( bindings::ValueVariant& val)
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

void TermArrayFilter::skip()
{
	const filter::StateTable::Element& st = g_struct_statetable[ m_state];
	m_state = st.skipState;
}

BindingFilterInterface* TermArrayFilter::createCopy() const
{
	return new TermArrayFilter(*this);
}

