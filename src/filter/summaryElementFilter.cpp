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
/// \file summaryElementFilter.cpp
#include "summaryElementFilter.hpp"
#include "structElementArray.hpp"
#include "stateTable.hpp"
#include "variantValueTemplate.hpp"

using namespace strus;

static const char* g_element_names[] = { "name","value","weight","index", 0};
static const filter::StructElementArray g_struct_elements( g_element_names);

enum TermState {
	StateEnd,
	StateNameOpen,
	StateNameValue,
	StateNameClose,
	StateValueOpen,
	StateValueValue,
	StateValueClose,
	StateWeightOpen,
	StateWeightValue,
	StateWeightClose,
	StateIndexOpen,
	StateIndexValue,
	StateIndexClose
};

enum TermArrayState {
	StateArrayEnd,
	StateArrayIndex,
	StateArrayNameOpen,
	StateArrayNameValue,
	StateArrayNameClose,
	StateArrayValueOpen,
	StateArrayValueValue,
	StateArrayValueClose,
	StateArrayWeightOpen,
	StateArrayWeightValue,
	StateArrayWeightClose,
	StateArrayIndexOpen,
	StateArrayIndexValue,
	StateArrayIndexClose
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
	
	{StateNameOpen,	_OPEN,  StateNameValue,		StateValueOpen,	_TAG,	 0, 0},
	{StateNameValue,	_VALUE, StateNameClose,		StateValueOpen,	_ELEM,	 0, 0},
	{StateNameClose,	_CLOSE, StateValueOpen,	StateValueOpen,	_NULL,	 0, 0},
	
	{StateValueOpen,	_OPEN,  StateValueValue,		StateWeightOpen,	_TAG,	 0, 1},
	{StateValueValue,	_VALUE, StateValueClose,		StateWeightOpen,	_ELEM,	 0, 1},
	{StateValueClose,	_CLOSE, StateWeightOpen,	StateWeightOpen,	_NULL,	 0, 1},
	
	{StateWeightOpen,	_OPEN,  StateWeightValue,		StateIndexOpen,	_TAG,	 0, 2},
	{StateWeightValue,	_VALUE, StateWeightClose,		StateIndexOpen,	_ELEM,	 0, 2},
	{StateWeightClose,	_CLOSE, StateIndexOpen,	StateIndexOpen,	_NULL,	 0, 2},
	
	{StateIndexOpen,	_OPEN,  StateIndexValue,		StateEnd,	_TAG,	 0, 3},
	{StateIndexValue,	_VALUE, StateIndexClose,		StateEnd,	_ELEM,	 0, 3},
	{StateIndexClose,	_CLOSE, StateEnd,	StateEnd,	_NULL,	 0, 3},
	
};

static const filter::StateTable::Element g_array_statetable[] = {
	{StateArrayEnd,		_CLOSE, StateArrayEnd,		StateArrayEnd,		_NULL,	 0, 0},
	{StateArrayIndex,	_INDEX, StateArrayNameOpen,	StateArrayIndex,	_TAG,	 1, 0},
	
	{StateArrayNameOpen,	_OPEN,  StateArrayNameValue,		StateArrayValueOpen,	_TAG,	 0, 0},
	{StateArrayNameValue,	_VALUE, StateArrayNameClose,		StateArrayValueOpen,	_ELEM,	 0, 0},
	{StateArrayNameClose,	_CLOSE, StateArrayValueOpen,	StateArrayValueOpen,	_NULL,	 0, 0},
	
	{StateArrayValueOpen,	_OPEN,  StateArrayValueValue,		StateArrayWeightOpen,	_TAG,	 0, 1},
	{StateArrayValueValue,	_VALUE, StateArrayValueClose,		StateArrayWeightOpen,	_ELEM,	 0, 1},
	{StateArrayValueClose,	_CLOSE, StateArrayWeightOpen,	StateArrayWeightOpen,	_NULL,	 0, 1},
	
	{StateArrayWeightOpen,	_OPEN,  StateArrayWeightValue,		StateArrayIndexOpen,	_TAG,	 0, 2},
	{StateArrayWeightValue,	_VALUE, StateArrayWeightClose,		StateArrayIndexOpen,	_ELEM,	 0, 2},
	{StateArrayWeightClose,	_CLOSE, StateArrayIndexOpen,	StateArrayIndexOpen,	_NULL,	 0, 2},
	
	{StateArrayIndexOpen,	_OPEN,  StateArrayIndexValue,		StateArrayIndex,	_TAG,	 0, 3},
	{StateArrayIndexValue,	_VALUE, StateArrayIndexClose,		StateArrayIndex,	_ELEM,	 0, 3},
	{StateArrayIndexClose,	_CLOSE, StateArrayIndex,	StateArrayIndex,	_NULL,	 0, 3},
	
};

SummaryElementFilter::SummaryElementFilter()
	:m_impl(0),m_ownership(0),m_state(0){}

SummaryElementFilter::SummaryElementFilter( const SummaryElementFilter& o)
	:m_impl(o.m_impl),m_ownership(o.m_ownership),m_state(o.m_state){}

SummaryElementFilter::SummaryElementFilter( const SummaryElement* impl)
	:m_impl(impl),m_ownership(0),m_state(1){}

SummaryElementFilter::SummaryElementFilter( SummaryElement* impl, bool withOwnership)
	:m_impl(impl),m_ownership(impl),m_state(1){}

SummaryElementFilter::~SummaryElementFilter()
{
	if (m_ownership) delete( m_ownership);
}

static bindings::ValueVariant getElementValue( const SummaryElement& elem, int valueIndex)
{
	switch (valueIndex) {

		case 0:
			return filter::VariantValueTemplate<std::string>::get( elem.name());

		case 1:
			return filter::VariantValueTemplate<std::string>::get( elem.value());

		case 2:
			return filter::VariantValueTemplate<double>::get( elem.weight());

		case 3:
			return filter::VariantValueTemplate<int>::get( elem.index());

	}
	return bindings::ValueVariant();
}

BindingFilterInterface::Tag SummaryElementFilter::getNext( bindings::ValueVariant& val)
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

void SummaryElementFilter::skip()
{
	const filter::StateTable::Element& st = g_struct_statetable[ m_state];
	m_state = st.skipState;
}

BindingFilterInterface* SummaryElementFilter::createCopy() const
{
	return new SummaryElementFilter(*this);
}

SummaryElementVectorFilter::SummaryElementVectorFilter()
	:m_impl(0),m_ownership(0),m_state(0),m_index(0){}

SummaryElementVectorFilter::SummaryElementVectorFilter( const SummaryElementVectorFilter& o)
	:m_impl(o.m_impl),m_ownership(o.m_ownership),m_state(o.m_state),m_index(o.m_index){}

SummaryElementVectorFilter::SummaryElementVectorFilter( const std::vector<SummaryElement>* impl)
	:m_impl(impl),m_ownership(0),m_state(1),m_index(0){}

SummaryElementVectorFilter::SummaryElementVectorFilter( std::vector<SummaryElement>* impl, bool withOwnership)
	:m_impl(impl),m_ownership(impl),m_state(1),m_index(0){}


BindingFilterInterface::Tag SummaryElementVectorFilter::getNext( bindings::ValueVariant& val)
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

void SummaryElementVectorFilter::skip()
{
	const filter::StateTable::Element& st = g_array_statetable[ m_state];
	m_state = st.skipState;
	if (m_state == StateArrayIndex && m_impl && m_index < m_impl->size())
	{
		++m_index;
	}
}

BindingFilterInterface* SummaryElementVectorFilter::createCopy() const
{
	return new SummaryElementVectorFilter(*this);
}
