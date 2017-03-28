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
/// \file queryElementFilter.cpp
#include "queryElementFilter.hpp"
#include "structElementArray.hpp"
#include "stateTable.hpp"
#include "variantValueTemplate.hpp"

using namespace strus;

static const char* g_element_names[] = { "type","idx","position","length","fieldNo", 0};
static const filter::StructElementArray g_struct_elements( g_element_names);

enum TermState {
	StateEnd,
	StateTypeOpen,
	StateTypeValue,
	StateTypeClose,
	StateIdxOpen,
	StateIdxValue,
	StateIdxClose,
	StatePositionOpen,
	StatePositionValue,
	StatePositionClose,
	StateLengthOpen,
	StateLengthValue,
	StateLengthClose,
	StateFieldNoOpen,
	StateFieldNoValue,
	StateFieldNoClose
};

enum TermArrayState {
	StateArrayEnd,
	StateArrayIndex,
	StateArrayTypeOpen,
	StateArrayTypeValue,
	StateArrayTypeClose,
	StateArrayIdxOpen,
	StateArrayIdxValue,
	StateArrayIdxClose,
	StateArrayPositionOpen,
	StateArrayPositionValue,
	StateArrayPositionClose,
	StateArrayLengthOpen,
	StateArrayLengthValue,
	StateArrayLengthClose,
	StateArrayFieldNoOpen,
	StateArrayFieldNoValue,
	StateArrayFieldNoClose
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
	{StateTypeOpen, _OPEN, StateTypeValue, StateIdxOpen, _TAG, 0, 0},
	{StateTypeValue, _VALUE, StateTypeClose, StateIdxOpen, _ELEM, 0, 0},
	{StateTypeClose, _CLOSE, StateIdxOpen, StateIdxOpen, _NULL, 0, 0},
	{StateIdxOpen, _OPEN, StateIdxValue, StatePositionOpen, _TAG, 0, 1},
	{StateIdxValue, _VALUE, StateIdxClose, StatePositionOpen, _ELEM, 0, 1},
	{StateIdxClose, _CLOSE, StatePositionOpen, StatePositionOpen, _NULL, 0, 1},
	{StatePositionOpen, _OPEN, StatePositionValue, StateLengthOpen, _TAG, 0, 2},
	{StatePositionValue, _VALUE, StatePositionClose, StateLengthOpen, _ELEM, 0, 2},
	{StatePositionClose, _CLOSE, StateLengthOpen, StateLengthOpen, _NULL, 0, 2},
	{StateLengthOpen, _OPEN, StateLengthValue, StateFieldNoOpen, _TAG, 0, 3},
	{StateLengthValue, _VALUE, StateLengthClose, StateFieldNoOpen, _ELEM, 0, 3},
	{StateLengthClose, _CLOSE, StateFieldNoOpen, StateFieldNoOpen, _NULL, 0, 3},
	{StateFieldNoOpen, _OPEN, StateFieldNoValue, StateEnd, _TAG, 0, 4},
	{StateFieldNoValue, _VALUE, StateFieldNoClose, StateEnd, _ELEM, 0, 4},
	{StateFieldNoClose, _CLOSE, StateEnd, StateEnd, _NULL, 0, 4}
};

static const filter::StateTable::Element g_array_statetable[] = {
	{StateArrayEnd, _CLOSE, StateArrayEnd, StateArrayEnd, _NULL, 0, 0},
	{StateArrayIndex, _INDEX, StateArrayTypeOpen, StateArrayIndex, _TAG, 1, 0},
	{StateArrayTypeOpen, _OPEN, StateArrayTypeValue, StateArrayIdxOpen, _TAG, 0, 0},
	{StateArrayTypeValue, _VALUE, StateArrayTypeClose, StateArrayIdxOpen, _ELEM, 0, 0},
	{StateArrayTypeClose, _CLOSE, StateArrayIdxOpen, StateArrayIdxOpen, _NULL, 0, 0},
	{StateArrayIdxOpen, _OPEN, StateArrayIdxValue, StateArrayPositionOpen, _TAG, 0, 1},
	{StateArrayIdxValue, _VALUE, StateArrayIdxClose, StateArrayPositionOpen, _ELEM, 0, 1},
	{StateArrayIdxClose, _CLOSE, StateArrayPositionOpen, StateArrayPositionOpen, _NULL, 0, 1},
	{StateArrayPositionOpen, _OPEN, StateArrayPositionValue, StateArrayLengthOpen, _TAG, 0, 2},
	{StateArrayPositionValue, _VALUE, StateArrayPositionClose, StateArrayLengthOpen, _ELEM, 0, 2},
	{StateArrayPositionClose, _CLOSE, StateArrayLengthOpen, StateArrayLengthOpen, _NULL, 0, 2},
	{StateArrayLengthOpen, _OPEN, StateArrayLengthValue, StateArrayFieldNoOpen, _TAG, 0, 3},
	{StateArrayLengthValue, _VALUE, StateArrayLengthClose, StateArrayFieldNoOpen, _ELEM, 0, 3},
	{StateArrayLengthClose, _CLOSE, StateArrayFieldNoOpen, StateArrayFieldNoOpen, _NULL, 0, 3},
	{StateArrayFieldNoOpen, _OPEN, StateArrayFieldNoValue, StateArrayIndex, _TAG, 0, 4},
	{StateArrayFieldNoValue, _VALUE, StateArrayFieldNoClose, StateArrayIndex, _ELEM, 0, 4},
	{StateArrayFieldNoClose, _CLOSE, StateArrayIndex, StateArrayIndex, _NULL, 0, 4},
};

QueryElementFilter::QueryElementFilter()
	:m_impl(0),m_ownership(0),m_state(0){}

QueryElementFilter::QueryElementFilter( const QueryElementFilter& o)
	:m_impl(o.m_impl),m_ownership(o.m_ownership),m_state(o.m_state){}

QueryElementFilter::QueryElementFilter( const analyzer::Query::Element* impl)
	:m_impl(impl),m_ownership(0),m_state(1){}

QueryElementFilter::QueryElementFilter( analyzer::Query::Element* impl, bool withOwnership)
	:m_impl(impl),m_ownership(impl),m_state(1){}

QueryElementFilter::~QueryElementFilter()
{
	if (m_ownership) delete( m_ownership);
}

static bindings::ValueVariant getElementValue( const analyzer::Query::Element& elem, int valueIndex)
{
	switch (valueIndex) {

		case 0:
			return filter::VariantValueTemplate<analyzer::Query::Element::Type>::get( elem.type());

		case 1:
			return filter::VariantValueTemplate<unsigned int>::get( elem.idx());

		case 2:
			return filter::VariantValueTemplate<unsigned int>::get( elem.position());

		case 3:
			return filter::VariantValueTemplate<unsigned int>::get( elem.length());

		case 4:
			return filter::VariantValueTemplate<unsigned int>::get( elem.fieldNo());

	}
	return bindings::ValueVariant();
}

BindingFilterInterface::Tag QueryElementFilter::getNext( bindings::ValueVariant& val)
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

void QueryElementFilter::skip()
{
	const filter::StateTable::Element& st = g_struct_statetable[ m_state];
	m_state = st.skipState;
}

BindingFilterInterface* QueryElementFilter::createCopy() const
{
	return new QueryElementFilter(*this);
}

QueryElementVectorFilter::QueryElementVectorFilter()
	:m_impl(0),m_ownership(0),m_state(0),m_index(0){}

QueryElementVectorFilter::QueryElementVectorFilter( const QueryElementVectorFilter& o)
	:m_impl(o.m_impl),m_ownership(o.m_ownership),m_state(o.m_state),m_index(o.m_index){}

QueryElementVectorFilter::QueryElementVectorFilter( const std::vector<analyzer::Query::Element>* impl)
	:m_impl(impl),m_ownership(0),m_state(1),m_index(0){}

QueryElementVectorFilter::QueryElementVectorFilter( std::vector<analyzer::Query::Element>* impl, bool withOwnership)
	:m_impl(impl),m_ownership(impl),m_state(1),m_index(0){}


BindingFilterInterface::Tag QueryElementVectorFilter::getNext( bindings::ValueVariant& val)
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

void QueryElementVectorFilter::skip()
{
	const filter::StateTable::Element& st = g_array_statetable[ m_state];
	m_state = st.skipState;
	if (m_state == StateArrayIndex && m_impl && m_index < m_impl->size())
	{
		++m_index;
	}
}

BindingFilterInterface* QueryElementVectorFilter::createCopy() const
{
	return new QueryElementVectorFilter(*this);
}
