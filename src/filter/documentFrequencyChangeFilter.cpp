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
/// \file documentFrequencyChangeFilter.cpp
#include "documentFrequencyChangeFilter.hpp"
#include "structElementArray.hpp"
#include "stateTable.hpp"
#include "variantValueTemplate.hpp"

using namespace strus;

static const char* g_element_names[] = { "type","value","increment", 0};
static const filter::StructElementArray g_struct_elements( g_element_names);

enum TermState {
	StateEnd,
	StateTypeOpen,
	StateTypeValue,
	StateTypeClose,
	StateValueOpen,
	StateValueValue,
	StateValueClose,
	StateIncrementOpen,
	StateIncrementValue,
	StateIncrementClose
};

enum TermArrayState {
	StateArrayEnd,
	StateArrayIndex,
	StateArrayTypeOpen,
	StateArrayTypeValue,
	StateArrayTypeClose,
	StateArrayValueOpen,
	StateArrayValueValue,
	StateArrayValueClose,
	StateArrayIncrementOpen,
	StateArrayIncrementValue,
	StateArrayIncrementClose
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
	{StateEnd, _CLOSE, StateEnd, StateEnd, _NULL, 0, 0},
	{StateTypeOpen, _OPEN, StateTypeValue, StateValueOpen, _TAG, 0, 0},
	{StateTypeValue, _VALUE, StateTypeClose, StateValueOpen, _ELEM, 0, 0},
	{StateTypeClose, _CLOSE, StateValueOpen, StateValueOpen, _NULL, 0, 0},
	{StateValueOpen, _OPEN, StateValueValue, StateIncrementOpen, _TAG, 0, 1},
	{StateValueValue, _VALUE, StateValueClose, StateIncrementOpen, _ELEM, 0, 1},
	{StateValueClose, _CLOSE, StateIncrementOpen, StateIncrementOpen, _NULL, 0, 1},
	{StateIncrementOpen, _OPEN, StateIncrementValue, StateEnd, _TAG, 0, 2},
	{StateIncrementValue, _VALUE, StateIncrementClose, StateEnd, _ELEM, 0, 2},
	{StateIncrementClose, _CLOSE, StateEnd, StateEnd, _NULL, 0, 2}
};

static const filter::StateTable::Element g_array_statetable[] = {
	{StateArrayEnd, _CLOSE, StateArrayEnd, StateArrayEnd, _NULL, 0, 0},
	{StateArrayIndex, _INDEX, StateArrayTypeOpen, StateArrayIndex, _TAG, 1, 0},
	{StateArrayTypeOpen, _OPEN, StateArrayTypeValue, StateArrayValueOpen, _TAG, 0, 0},
	{StateArrayTypeValue, _VALUE, StateArrayTypeClose, StateArrayValueOpen, _ELEM, 0, 0},
	{StateArrayTypeClose, _CLOSE, StateArrayValueOpen, StateArrayValueOpen, _NULL, 0, 0},
	{StateArrayValueOpen, _OPEN, StateArrayValueValue, StateArrayIncrementOpen, _TAG, 0, 1},
	{StateArrayValueValue, _VALUE, StateArrayValueClose, StateArrayIncrementOpen, _ELEM, 0, 1},
	{StateArrayValueClose, _CLOSE, StateArrayIncrementOpen, StateArrayIncrementOpen, _NULL, 0, 1},
	{StateArrayIncrementOpen, _OPEN, StateArrayIncrementValue, StateArrayIndex, _TAG, 0, 2},
	{StateArrayIncrementValue, _VALUE, StateArrayIncrementClose, StateArrayIndex, _ELEM, 0, 2},
	{StateArrayIncrementClose, _CLOSE, StateArrayIndex, StateArrayIndex, _NULL, 0, 2},
};

DocumentFrequencyChangeFilter::DocumentFrequencyChangeFilter()
	:m_impl(0),m_ownership(0),m_state(0){}

DocumentFrequencyChangeFilter::DocumentFrequencyChangeFilter( const DocumentFrequencyChangeFilter& o)
	:m_impl(o.m_impl),m_ownership(o.m_ownership),m_state(o.m_state){}

DocumentFrequencyChangeFilter::DocumentFrequencyChangeFilter( const bindings::DocumentFrequencyChange* impl)
	:m_impl(impl),m_ownership(0),m_state(1){}

DocumentFrequencyChangeFilter::DocumentFrequencyChangeFilter( bindings::DocumentFrequencyChange* impl, bool withOwnership)
	:m_impl(impl),m_ownership(impl),m_state(1){}

DocumentFrequencyChangeFilter::~DocumentFrequencyChangeFilter()
{
	if (m_ownership) delete( m_ownership);
}

static bindings::ValueVariant getElementValue( const bindings::DocumentFrequencyChange& elem, int valueIndex)
{
	switch (valueIndex) {

		case 0:
			return filter::VariantValueTemplate<const char*>::get( elem.type());

		case 1:
			return filter::VariantValueTemplate<const char*>::get( elem.value());

		case 2:
			return filter::VariantValueTemplate<int>::get( elem.increment());

	}
	return bindings::ValueVariant();
}

BindingFilterInterface::Tag DocumentFrequencyChangeFilter::getNext( bindings::ValueVariant& val)
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

void DocumentFrequencyChangeFilter::skip()
{
	const filter::StateTable::Element& st = g_struct_statetable[ m_state];
	m_state = st.skipState;
}

BindingFilterInterface* DocumentFrequencyChangeFilter::createCopy() const
{
	return new DocumentFrequencyChangeFilter(*this);
}

DocumentFrequencyChangeVectorFilter::DocumentFrequencyChangeVectorFilter()
	:m_impl(0),m_ownership(0),m_state(0),m_index(0){}

DocumentFrequencyChangeVectorFilter::DocumentFrequencyChangeVectorFilter( const DocumentFrequencyChangeVectorFilter& o)
	:m_impl(o.m_impl),m_ownership(o.m_ownership),m_state(o.m_state),m_index(o.m_index){}

DocumentFrequencyChangeVectorFilter::DocumentFrequencyChangeVectorFilter( const std::vector<bindings::DocumentFrequencyChange>* impl)
	:m_impl(impl),m_ownership(0),m_state(1),m_index(0){}

DocumentFrequencyChangeVectorFilter::DocumentFrequencyChangeVectorFilter( std::vector<bindings::DocumentFrequencyChange>* impl, bool withOwnership)
	:m_impl(impl),m_ownership(impl),m_state(1),m_index(0){}


BindingFilterInterface::Tag DocumentFrequencyChangeVectorFilter::getNext( bindings::ValueVariant& val)
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

void DocumentFrequencyChangeVectorFilter::skip()
{
	const filter::StateTable::Element& st = g_array_statetable[ m_state];
	m_state = st.skipState;
	if (m_state == StateArrayIndex && m_impl && m_index < m_impl->size())
	{
		++m_index;
	}
}

BindingFilterInterface* DocumentFrequencyChangeVectorFilter::createCopy() const
{
	return new DocumentFrequencyChangeVectorFilter(*this);
}
