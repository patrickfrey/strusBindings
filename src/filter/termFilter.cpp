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
/// \file termFilter.cpp
#include "termFilter.hpp"
#include "filter/structElementArray.hpp"

using namespace strus;

static const char* g_element_names[] = "type", "value", "pos", "len", 0};
static const filter::StructElementArray g_struct_elements( g_element_names);

enum TermState {
	StateEnd,
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

enum TermArrayState {
	StateArrayEnd,
	StateArrayIndex,
	StateArrayTypeOpen,
	StateArrayTypeValue,
	StateArrayTypeClose,
	StateArrayValueOpen,
	StateArrayValueValue,
	StateArrayValueClose,
	StateArrayPosOpen,
	StateArrayPosValue,
	StateArrayPosClose,
	StateArrayLenOpen,
	StateArrayLenValue,
	StateArrayLenClose
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
	
	{StateTypeOpen,	_OPEN,  StateTypeValue,		StateValueOpen,	_TAG,	 0, 0},
	{StateTypeValue,	_VALUE, StateTypeClose,		StateValueOpen,	_ELEM,	 0, 0},
	{StateTypeClose,	_CLOSE, StateValueOpen,	StateValueOpen,	_NULL,	 0, 0},
	
	{StateValueOpen,	_OPEN,  StateValueValue,		StatePosOpen,	_TAG,	 0, 1},
	{StateValueValue,	_VALUE, StateValueClose,		StatePosOpen,	_ELEM,	 0, 1},
	{StateValueClose,	_CLOSE, StatePosOpen,	StatePosOpen,	_NULL,	 0, 1},
	
	{StatePosOpen,	_OPEN,  StatePosValue,		StateLenOpen,	_TAG,	 0, 2},
	{StatePosValue,	_VALUE, StatePosClose,		StateLenOpen,	_ELEM,	 0, 2},
	{StatePosClose,	_CLOSE, StateLenOpen,	StateLenOpen,	_NULL,	 0, 2},
	
	{StateLenOpen,	_OPEN,  StateLenValue,		StateEnd,	_TAG,	 0, 3},
	{StateLenValue,	_VALUE, StateLenClose,		StateEnd,	_ELEM,	 0, 3},
	{StateLenClose,	_CLOSE, StateEnd,	StateEnd,	_NULL,	 0, 3},
	
};

static const filter::StateTable::Element g_array_statetable[] = {
	{StateArrayEnd,		_CLOSE, StateArrayEnd,		StateArrayEnd,		_NULL,	 0, 0},
	{StateArrayIndex,	_INDEX, StateArrayTypeOpen,	StateArrayIndex,	_TAG,	 1, 0},
	
	{StateArrayTypeOpen,	_OPEN,  StateArrayTypeValue,		StateArrayValueOpen,	_TAG,	 0, 0},
	{StateArrayTypeValue,	_VALUE, StateArrayTypeClose,		StateArrayValueOpen,	_ELEM,	 0, 0},
	{StateArrayTypeClose,	_CLOSE, StateArrayValueOpen,	StateArrayValueOpen,	_NULL,	 0, 0},
	
	{StateArrayValueOpen,	_OPEN,  StateArrayValueValue,		StateArrayPosOpen,	_TAG,	 0, 1},
	{StateArrayValueValue,	_VALUE, StateArrayValueClose,		StateArrayPosOpen,	_ELEM,	 0, 1},
	{StateArrayValueClose,	_CLOSE, StateArrayPosOpen,	StateArrayPosOpen,	_NULL,	 0, 1},
	
	{StateArrayPosOpen,	_OPEN,  StateArrayPosValue,		StateArrayLenOpen,	_TAG,	 0, 2},
	{StateArrayPosValue,	_VALUE, StateArrayPosClose,		StateArrayLenOpen,	_ELEM,	 0, 2},
	{StateArrayPosClose,	_CLOSE, StateArrayLenOpen,	StateArrayLenOpen,	_NULL,	 0, 2},
	
	{StateArrayLenOpen,	_OPEN,  StateArrayLenValue,		StateArrayIndex,	_TAG,	 0, 3},
	{StateArrayLenValue,	_VALUE, StateArrayLenClose,		StateArrayIndex,	_ELEM,	 0, 3},
	{StateArrayLenClose,	_CLOSE, StateArrayIndex,	StateArrayIndex,	_NULL,	 0, 3},
	
};

TermFilter::TermFilter()
	:m_impl(0),m_ownership(0),m_state(0){}

TermFilter::TermFilter( const TermFilter& o)
	:m_impl(o.m_impl),m_ownership(o.m_ownership),m_state(o.m_state){}

TermFilter::TermFilter( const analyzer::Term* impl)
	:m_impl(impl),m_ownership(0),m_state(1){}

TermFilter::TermFilter( analyzer::Term* impl, bool withOwnership)
	:m_impl(impl),m_ownership(impl),m_state(1){}

TermFilter::~TermFilter()
{
	if (m_ownership) delete( m_ownership);
}

static binding::ValueVariant getElementValue( const analyzer::Term& elem, int valueIndex)
{
	switch (valueIndex) {
	
		case 0:
			return binding::ValueVariant( elem.type().c_str(), elem.type().size());
		
		case 1:
			return binding::ValueVariant( elem.value().c_str(), elem.value().size());
		
		case 2:
			return binding::ValueVariant( (binding::ValueVariant::UIntType)elem.pos());
		
		case 3:
			return binding::ValueVariant( (binding::ValueVariant::UIntType)elem.len());
		
	}
	return binding::ValueVariant();
}

BindingFilterInterface::Tag TermFilter::getNext( binding::ValueVariant& val)
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

void TermFilter::skip()
{
	const filter::StateTable::Element& st = g_struct_statetable[ m_state];
	m_state = st.skipState;
}

BindingFilterInterface* TermFilter::createCopy() const
{
	return new TermFilter(*this);
}

TermVectorFilter::TermVectorFilter()
	:m_impl(0),m_ownership(0),m_state(0),m_index(0){}

TermVectorFilter::TermVectorFilter( const TermVectorFilter& o)
	:m_impl(o.m_impl),m_ownership(o.m_ownership),m_state(o.m_state),m_index(o.m_index){}

TermVectorFilter::TermVectorFilter( const std::vector<analyzer::Term>* impl)
	:m_impl(impl),m_ownership(0),m_state(1),m_index(0){}

TermVectorFilter::TermVectorFilter( std::vector<analyzer::Term>* impl, bool withOwnership)
	:m_impl(impl),m_ownership(impl),m_state(1),m_index(0){}


BindingFilterInterface::Tag TermVectorFilter::getNext( binding::ValueVariant& val)
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

void TermVectorFilter::skip()
{
	const filter::StateTable::Element& st = g_array_statetable[ m_state];
	m_state = st.skipState;
	if (m_state == StateArrayIndex && m_impl && m_index < m_impl->size())
	{
		++m_index;
	}
}

BindingFilterInterface* TermVectorFilter::createCopy() const
{
	return new TermVectorFilter(*this);
}
