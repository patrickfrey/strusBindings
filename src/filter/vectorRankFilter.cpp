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
/// \file vectorRankFilter.cpp
#include "vectorRankFilter.hpp"
#include "structElementArray.hpp"
#include "stateTable.hpp"
#include "variantValueTemplate.hpp"

using namespace strus;

static const char* g_element_names[] = { "featidx","weight", 0};
static const filter::StructElementArray g_struct_elements( g_element_names);

enum TermState {
	StateEnd,
	StateFeatidxOpen,
	StateFeatidxValue,
	StateFeatidxClose,
	StateWeightOpen,
	StateWeightValue,
	StateWeightClose
};

enum TermArrayState {
	StateArrayEnd,
	StateIndex,
	StateFeatidxOpen,
	StateFeatidxValue,
	StateFeatidxClose,
	StateWeightOpen,
	StateWeightValue,
	StateWeightClose
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
	{StateFeatidxOpen, _OPEN, StateFeatidxValue, StateWeightOpen, _TAG, 0, -1},
	{StateFeatidxValue, _VALUE, StateFeatidxClose, StateFeatidxClose, _ELEM, -1, 0},
	{StateFeatidxClose, _CLOSE, StateWeightOpen, StateWeightOpen, _NULL, -1, -1},
	{StateWeightOpen, _OPEN, StateWeightValue, StateEnd, _TAG, 1, -1},
	{StateWeightValue, _VALUE, StateWeightClose, StateWeightClose, _ELEM, -1, 1},
	{StateWeightClose, _CLOSE, StateEnd, StateEnd, _NULL, -1, -1}
};

static const filter::StateTable::Element g_array_statetable[] = {
	{StateArrayEnd, _CLOSE, StateArrayEnd, StateArrayEnd, _NULL, 0, 0},
	{StateArrayIndex, _INDEX, StateArrayFeatidxOpen, StateArrayIndex, _TAG, 1, 0},
	{StateArrayFeatidxOpen, _OPEN, StateArrayFeatidxValue, StateArrayWeightOpen, _TAG, 0, -1},
	{StateArrayFeatidxValue, _VALUE, StateArrayFeatidxClose, StateArrayFeatidxClose, _ELEM, -1, 0},
	{StateArrayFeatidxClose, _CLOSE, StateArrayWeightOpen, StateArrayWeightOpen, _NULL, -1, -1},
	{StateArrayWeightOpen, _OPEN, StateArrayWeightValue, StateArrayIndex, _TAG, 1, -1},
	{StateArrayWeightValue, _VALUE, StateArrayWeightClose, StateArrayWeightClose, _ELEM, -1, 1},
	{StateArrayWeightClose, _CLOSE, StateArrayIndex, StateArrayIndex, _NULL, -1, -1}
};

VectorRankFilter::VectorRankFilter()
	:m_impl(0),m_ownership(0),m_state(0){}

VectorRankFilter::VectorRankFilter( const VectorRankFilter& o)
	:m_impl(o.m_impl),m_ownership(o.m_ownership),m_state(o.m_state){}

VectorRankFilter::VectorRankFilter( const VectorStorageSearchInterface::Result* impl)
	:m_impl(impl),m_ownership(0),m_state(1){}

VectorRankFilter::VectorRankFilter( VectorStorageSearchInterface::Result* impl, bool withOwnership)
	:m_impl(impl),m_ownership(impl),m_state(1){}

VectorRankFilter::~VectorRankFilter()
{
	if (m_ownership) delete( m_ownership);
}

static bindings::ValueVariant getElementValue( const VectorStorageSearchInterface::Result& elem, int valueIndex)
{
	switch (valueIndex) {

		case 0:
			return bindings::ValueVariant( (bindings::ValueVariant::IntType)(*m_impl).featidx());

		case 1:
			return bindings::ValueVariant( (*m_impl).weight());

	}
	return bindings::ValueVariant();
}

BindingFilterInterface::Tag VectorRankFilter::getNext( bindings::ValueVariant& val)
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

void VectorRankFilter::skip()
{
	const filter::StateTable::Element& st = g_struct_statetable[ m_state];
	m_state = st.skipState;
}

BindingFilterInterface* VectorRankFilter::createCopy() const
{
	return new VectorRankFilter(*this);
}

VectorRankVectorFilter::VectorRankVectorFilter()
	:m_impl(0),m_ownership(0),m_state(0),m_index(0){}

VectorRankVectorFilter::VectorRankVectorFilter( const VectorRankVectorFilter& o)
	:m_impl(o.m_impl),m_ownership(o.m_ownership),m_state(o.m_state),m_index(o.m_index){}

VectorRankVectorFilter::VectorRankVectorFilter( const std::vector<VectorStorageSearchInterface::Result>* impl)
	:m_impl(impl),m_ownership(0),m_state(1),m_index(0){}

VectorRankVectorFilter::VectorRankVectorFilter( std::vector<VectorStorageSearchInterface::Result>* impl, bool withOwnership)
	:m_impl(impl),m_ownership(impl),m_state(1),m_index(0){}


BindingFilterInterface::Tag VectorRankVectorFilter::getNext( bindings::ValueVariant& val)
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

void VectorRankVectorFilter::skip()
{
	const filter::StateTable::Element& st = g_array_statetable[ m_state];
	m_state = st.skipState;
	if (m_state == StateArrayIndex && m_impl && m_index < m_impl->size())
	{
		++m_index;
	}
}

BindingFilterInterface* VectorRankVectorFilter::createCopy() const
{
	return new VectorRankVectorFilter(*this);
}
