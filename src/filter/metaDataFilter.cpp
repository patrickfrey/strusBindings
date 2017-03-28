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
/// \file metaDataFilter.cpp
#include "metaDataFilter.hpp"
#include "structElementArray.hpp"
#include "stateTable.hpp"
#include "variantValueTemplate.hpp"

using namespace strus;

static const char* g_element_names[] = { "name","value", 0};
static const filter::StructElementArray g_struct_elements( g_element_names);

enum TermState {
	StateEnd,
	StateNameOpen,
	StateNameValue,
	StateNameClose,
	StateValueOpen,
	StateValueValue,
	StateValueClose
};

enum TermArrayState {
	StateArrayEnd,
	StateArrayIndex,
	StateArrayNameOpen,
	StateArrayNameValue,
	StateArrayNameClose,
	StateArrayValueOpen,
	StateArrayValueValue,
	StateArrayValueClose
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
	{StateNameOpen, _OPEN, StateNameValue, StateValueOpen, _TAG, 0, 0},
	{StateNameValue, _VALUE, StateNameClose, StateValueOpen, _ELEM, 0, 0},
	{StateNameClose, _CLOSE, StateValueOpen, StateValueOpen, _NULL, 0, 0},
	{StateValueOpen, _OPEN, StateValueValue, StateEnd, _TAG, 0, 1},
	{StateValueValue, _VALUE, StateValueClose, StateEnd, _ELEM, 0, 1},
	{StateValueClose, _CLOSE, StateEnd, StateEnd, _NULL, 0, 1}
};

static const filter::StateTable::Element g_array_statetable[] = {
	{StateArrayEnd, _CLOSE, StateArrayEnd, StateArrayEnd, _NULL, 0, 0},
	{StateArrayIndex, _INDEX, StateArrayNameOpen, StateArrayIndex, _TAG, 1, 0},
	{StateArrayNameOpen, _OPEN, StateArrayNameValue, StateArrayValueOpen, _TAG, 0, 0},
	{StateArrayNameValue, _VALUE, StateArrayNameClose, StateArrayValueOpen, _ELEM, 0, 0},
	{StateArrayNameClose, _CLOSE, StateArrayValueOpen, StateArrayValueOpen, _NULL, 0, 0},
	{StateArrayValueOpen, _OPEN, StateArrayValueValue, StateArrayIndex, _TAG, 0, 1},
	{StateArrayValueValue, _VALUE, StateArrayValueClose, StateArrayIndex, _ELEM, 0, 1},
	{StateArrayValueClose, _CLOSE, StateArrayIndex, StateArrayIndex, _NULL, 0, 1},
};

MetaDataFilter::MetaDataFilter()
	:m_impl(0),m_ownership(0),m_state(0){}

MetaDataFilter::MetaDataFilter( const MetaDataFilter& o)
	:m_impl(o.m_impl),m_ownership(o.m_ownership),m_state(o.m_state){}

MetaDataFilter::MetaDataFilter( const analyzer::MetaData* impl)
	:m_impl(impl),m_ownership(0),m_state(1){}

MetaDataFilter::MetaDataFilter( analyzer::MetaData* impl, bool withOwnership)
	:m_impl(impl),m_ownership(impl),m_state(1){}

MetaDataFilter::~MetaDataFilter()
{
	if (m_ownership) delete( m_ownership);
}

static bindings::ValueVariant getElementValue( const analyzer::MetaData& elem, int valueIndex)
{
	switch (valueIndex) {

		case 0:
			return filter::VariantValueTemplate<std::string>::get( elem.name());

		case 1:
			return filter::VariantValueTemplate<NumericVariant>::get( elem.value());

	}
	return bindings::ValueVariant();
}

BindingFilterInterface::Tag MetaDataFilter::getNext( bindings::ValueVariant& val)
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

void MetaDataFilter::skip()
{
	const filter::StateTable::Element& st = g_struct_statetable[ m_state];
	m_state = st.skipState;
}

BindingFilterInterface* MetaDataFilter::createCopy() const
{
	return new MetaDataFilter(*this);
}

MetaDataVectorFilter::MetaDataVectorFilter()
	:m_impl(0),m_ownership(0),m_state(0),m_index(0){}

MetaDataVectorFilter::MetaDataVectorFilter( const MetaDataVectorFilter& o)
	:m_impl(o.m_impl),m_ownership(o.m_ownership),m_state(o.m_state),m_index(o.m_index){}

MetaDataVectorFilter::MetaDataVectorFilter( const std::vector<analyzer::MetaData>* impl)
	:m_impl(impl),m_ownership(0),m_state(1),m_index(0){}

MetaDataVectorFilter::MetaDataVectorFilter( std::vector<analyzer::MetaData>* impl, bool withOwnership)
	:m_impl(impl),m_ownership(impl),m_state(1),m_index(0){}


BindingFilterInterface::Tag MetaDataVectorFilter::getNext( bindings::ValueVariant& val)
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

void MetaDataVectorFilter::skip()
{
	const filter::StateTable::Element& st = g_array_statetable[ m_state];
	m_state = st.skipState;
	if (m_state == StateArrayIndex && m_impl && m_index < m_impl->size())
	{
		++m_index;
	}
}

BindingFilterInterface* MetaDataVectorFilter::createCopy() const
{
	return new MetaDataVectorFilter(*this);
}
