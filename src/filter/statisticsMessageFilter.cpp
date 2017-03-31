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
/// \file statisticsMessageFilter.cpp
#include "statisticsMessageFilter.hpp"
#include "structElementArray.hpp"
#include "stateTable.hpp"
#include "variantValueTemplate.hpp"
#include <cstring>

using namespace strus;

static const char* g_element_names[] = { "df","type","value","increment","nofdocs", 0};
static const filter::StructElementArray g_struct_elements( g_element_names);

enum TermState {
	StateEnd,
	StateDfOpen,
	StateDfIndex,
	StateDfTypeOpen,
	StateDfTypeValue,
	StateDfTypeClose,
	StateDfValueOpen,
	StateDfValueValue,
	StateDfValueClose,
	StateDfIncrementOpen,
	StateDfIncrementValue,
	StateDfIncrementClose,
	StateDfClose,
	StateNofdocsOpen,
	StateNofdocsValue,
	StateNofdocsClose
};

enum TermArrayState {
	StateArrayEnd,
	StateIndex,
	StateDfOpen,
	StateDfIndex,
	StateDfTypeOpen,
	StateDfTypeValue,
	StateDfTypeClose,
	StateDfValueOpen,
	StateDfValueValue,
	StateDfValueClose,
	StateDfIncrementOpen,
	StateDfIncrementValue,
	StateDfIncrementClose,
	StateDfClose,
	StateNofdocsOpen,
	StateNofdocsValue,
	StateNofdocsClose
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
	{StateDfOpen, _OPEN, StateDfIndex, StateNofdocsOpen, _TAG, 0, -1},
	{StateDfIndex, _INDEX, StateDfTypeOpen, StateDfIndex, _TAG, -1, -1},
	{StateDfTypeOpen, _OPEN, StateDfTypeValue, StateDfValueOpen, _TAG, 1, -1},
	{StateDfTypeValue, _VALUE, StateDfTypeClose, StateDfTypeClose, _ELEM, -1, 0},
	{StateDfTypeClose, _CLOSE, StateDfValueOpen, StateDfValueOpen, _NULL, -1, -1},
	{StateDfValueOpen, _OPEN, StateDfValueValue, StateDfIncrementOpen, _TAG, 2, -1},
	{StateDfValueValue, _VALUE, StateDfValueClose, StateDfValueClose, _ELEM, -1, 1},
	{StateDfValueClose, _CLOSE, StateDfIncrementOpen, StateDfIncrementOpen, _NULL, -1, -1},
	{StateDfIncrementOpen, _OPEN, StateDfIncrementValue, StateDfIndex, _TAG, 3, -1},
	{StateDfIncrementValue, _VALUE, StateDfIncrementClose, StateDfIncrementClose, _ELEM, -1, 2},
	{StateDfIncrementClose, _CLOSE, StateDfIndex, StateDfIndex, _NULL, -1, -1},
	{StateDfClose, _CLOSE, StateNofdocsOpen, StateNofdocsOpen, _NULL, -1, -1},
	{StateNofdocsOpen, _OPEN, StateNofdocsValue, StateEnd, _TAG, 4, -1},
	{StateNofdocsValue, _VALUE, StateNofdocsClose, StateNofdocsClose, _ELEM, -1, 1},
	{StateNofdocsClose, _CLOSE, StateEnd, StateEnd, _NULL, -1, -1}
};

static const filter::StateTable::Element g_array_statetable[] = {
	{StateArrayEnd, _CLOSE, StateArrayEnd, StateArrayEnd, _NULL, 0, 0},
	{StateArrayIndex, _INDEX, StateArrayDfOpen, StateArrayIndex, _TAG, 1, 0},
	{StateArrayDfOpen, _OPEN, StateArrayDfIndex, StateArrayNofdocsOpen, _TAG, 0, -1},
	{StateArrayDfIndex, _INDEX, StateArrayDfTypeOpen, StateArrayDfIndex, _TAG, -1, -1},
	{StateArrayDfTypeOpen, _OPEN, StateArrayDfTypeValue, StateArrayDfValueOpen, _TAG, 1, -1},
	{StateArrayDfTypeValue, _VALUE, StateArrayDfTypeClose, StateArrayDfTypeClose, _ELEM, -1, 0},
	{StateArrayDfTypeClose, _CLOSE, StateArrayDfValueOpen, StateArrayDfValueOpen, _NULL, -1, -1},
	{StateArrayDfValueOpen, _OPEN, StateArrayDfValueValue, StateArrayDfIncrementOpen, _TAG, 2, -1},
	{StateArrayDfValueValue, _VALUE, StateArrayDfValueClose, StateArrayDfValueClose, _ELEM, -1, 1},
	{StateArrayDfValueClose, _CLOSE, StateArrayDfIncrementOpen, StateArrayDfIncrementOpen, _NULL, -1, -1},
	{StateArrayDfIncrementOpen, _OPEN, StateArrayDfIncrementValue, StateArrayDfIndex, _TAG, 3, -1},
	{StateArrayDfIncrementValue, _VALUE, StateArrayDfIncrementClose, StateArrayDfIncrementClose, _ELEM, -1, 2},
	{StateArrayDfIncrementClose, _CLOSE, StateArrayDfIndex, StateArrayDfIndex, _NULL, -1, -1},
	{StateArrayDfClose, _CLOSE, StateArrayNofdocsOpen, StateArrayNofdocsOpen, _NULL, -1, -1},
	{StateArrayNofdocsOpen, _OPEN, StateArrayNofdocsValue, StateArrayIndex, _TAG, 4, -1},
	{StateArrayNofdocsValue, _VALUE, StateArrayNofdocsClose, StateArrayNofdocsClose, _ELEM, -1, 1},
	{StateArrayNofdocsClose, _CLOSE, StateArrayIndex, StateArrayIndex, _NULL, -1, -1}
};

StatisticsMessageFilter::StatisticsMessageFilter()
	:m_impl(0),m_ownership(0),m_state(0)
{
	std::memset( m_index, 0, sizeof(m_index));
}

StatisticsMessageFilter::StatisticsMessageFilter( const StatisticsMessageFilter& o)
	:m_impl(o.m_impl),m_ownership(o.m_ownership),m_state(o.m_state)
{
	std::memcpy( m_index, o.m_index, sizeof(m_index));
}

StatisticsMessageFilter::StatisticsMessageFilter( const bindings::StatisticsMessage* impl)
	:m_impl(impl),m_ownership(0),m_state(1)
{
	std::memset( m_index, 0, sizeof(m_index));
}

StatisticsMessageFilter::StatisticsMessageFilter( bindings::StatisticsMessage* impl, bool withOwnership)
	:m_impl(impl),m_ownership(impl),m_state(1)
{
	std::memset( m_index, 0, sizeof(m_index));
}

StatisticsMessageFilter::~StatisticsMessageFilter()
{
	if (m_ownership) delete( m_ownership);
}

static bindings::ValueVariant getElementValue( const bindings::StatisticsMessage& elem, int valueIndex)
{
	switch (valueIndex) {

		case 0:
			return bindings::ValueVariant( (*m_impl).df()[m_index[0]].type());

		case 1:
			return bindings::ValueVariant( (*m_impl).df()[m_index[0]].value());

		case 2:
			return bindings::ValueVariant( (bindings::ValueVariant::IntType)(*m_impl).df()[m_index[0]].increment());

		case 3:
			return bindings::ValueVariant( (bindings::ValueVariant::IntType)(*m_impl).nofdocs());

		}
	return bindings::ValueVariant();
}

BindingFilterInterface::Tag StatisticsMessageFilter::getNext( bindings::ValueVariant& val)
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

void StatisticsMessageFilter::skip()
{
	const filter::StateTable::Element& st = g_struct_statetable[ m_state];
	m_state = st.skipState;
}

BindingFilterInterface* StatisticsMessageFilter::createCopy() const
{
	return new StatisticsMessageFilter(*this);
}

