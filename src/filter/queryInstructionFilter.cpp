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
/// \file queryInstructionFilter.cpp
#include "queryInstructionFilter.hpp"
#include "structElementArray.hpp"
#include "stateTable.hpp"
#include "variantValueTemplate.hpp"

using namespace strus;

static const char* g_element_names[] = { "opCode","idx","nofOperands", 0};
static const filter::StructElementArray g_struct_elements( g_element_names);

enum TermState {
	StateEnd,
	StateOpCodeOpen,
	StateOpCodeValue,
	StateOpCodeClose,
	StateIdxOpen,
	StateIdxValue,
	StateIdxClose,
	StateNofOperandsOpen,
	StateNofOperandsValue,
	StateNofOperandsClose
};

enum TermArrayState {
	StateArrayEnd,
	StateArrayIndex,
	StateArrayOpCodeOpen,
	StateArrayOpCodeValue,
	StateArrayOpCodeClose,
	StateArrayIdxOpen,
	StateArrayIdxValue,
	StateArrayIdxClose,
	StateArrayNofOperandsOpen,
	StateArrayNofOperandsValue,
	StateArrayNofOperandsClose
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
	{StateOpCodeOpen, _OPEN, StateOpCodeValue, StateIdxOpen, _TAG, 0, -1},
	{StateOpCodeValue, _VALUE, StateOpCodeClose, StateIdxOpen, _ELEM, -1, 0},
	{StateOpCodeClose, _CLOSE, StateIdxOpen, StateIdxOpen, _NULL, -1, -1},
	{StateIdxOpen, _OPEN, StateIdxValue, StateNofOperandsOpen, _TAG, 1, -1},
	{StateIdxValue, _VALUE, StateIdxClose, StateNofOperandsOpen, _ELEM, -1, 1},
	{StateIdxClose, _CLOSE, StateNofOperandsOpen, StateNofOperandsOpen, _NULL, -1, -1},
	{StateNofOperandsOpen, _OPEN, StateNofOperandsValue, StateEnd, _TAG, 2, -1},
	{StateNofOperandsValue, _VALUE, StateNofOperandsClose, StateEnd, _ELEM, -1, 2},
	{StateNofOperandsClose, _CLOSE, StateEnd, StateEnd, _NULL, -1, -1}
};

static const filter::StateTable::Element g_array_statetable[] = {
	{StateArrayEnd, _CLOSE, StateArrayEnd, StateArrayEnd, _NULL, 0, 0},
	{StateArrayIndex, _INDEX, StateArrayOpCodeOpen, StateArrayIndex, _TAG, 1, 0},
	{StateArrayOpCodeOpen, _OPEN, StateArrayOpCodeValue, StateArrayIdxOpen, _TAG, 0, -1},
	{StateArrayOpCodeValue, _VALUE, StateArrayOpCodeClose, StateArrayIdxOpen, _ELEM, -1, 0},
	{StateArrayOpCodeClose, _CLOSE, StateArrayIdxOpen, StateArrayIdxOpen, _NULL, -1, -1},
	{StateArrayIdxOpen, _OPEN, StateArrayIdxValue, StateArrayNofOperandsOpen, _TAG, 1, -1},
	{StateArrayIdxValue, _VALUE, StateArrayIdxClose, StateArrayNofOperandsOpen, _ELEM, -1, 1},
	{StateArrayIdxClose, _CLOSE, StateArrayNofOperandsOpen, StateArrayNofOperandsOpen, _NULL, -1, -1},
	{StateArrayNofOperandsOpen, _OPEN, StateArrayNofOperandsValue, StateArrayIndex, _TAG, 2, -1},
	{StateArrayNofOperandsValue, _VALUE, StateArrayNofOperandsClose, StateArrayIndex, _ELEM, -1, 2},
	{StateArrayNofOperandsClose, _CLOSE, StateArrayIndex, StateArrayIndex, _NULL, -1, -1}
};

QueryInstructionFilter::QueryInstructionFilter()
	:m_impl(0),m_ownership(0),m_state(0){}

QueryInstructionFilter::QueryInstructionFilter( const QueryInstructionFilter& o)
	:m_impl(o.m_impl),m_ownership(o.m_ownership),m_state(o.m_state){}

QueryInstructionFilter::QueryInstructionFilter( const analyzer::Query::Instruction* impl)
	:m_impl(impl),m_ownership(0),m_state(1){}

QueryInstructionFilter::QueryInstructionFilter( analyzer::Query::Instruction* impl, bool withOwnership)
	:m_impl(impl),m_ownership(impl),m_state(1){}

QueryInstructionFilter::~QueryInstructionFilter()
{
	if (m_ownership) delete( m_ownership);
}

static bindings::ValueVariant getElementValue( const analyzer::Query::Instruction& elem, int valueIndex)
{
	switch (valueIndex) {

		case 0:
			return filter::VariantValueTemplate<analyzer::Query::Instruction::OpCode>::get( elem.opCode());

		case 1:
			return filter::VariantValueTemplate<unsigned int>::get( elem.idx());

		case 2:
			return filter::VariantValueTemplate<unsigned int>::get( elem.nofOperands());

	}
	return bindings::ValueVariant();
}

BindingFilterInterface::Tag QueryInstructionFilter::getNext( bindings::ValueVariant& val)
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

void QueryInstructionFilter::skip()
{
	const filter::StateTable::Element& st = g_struct_statetable[ m_state];
	m_state = st.skipState;
}

BindingFilterInterface* QueryInstructionFilter::createCopy() const
{
	return new QueryInstructionFilter(*this);
}

QueryInstructionVectorFilter::QueryInstructionVectorFilter()
	:m_impl(0),m_ownership(0),m_state(0),m_index(0){}

QueryInstructionVectorFilter::QueryInstructionVectorFilter( const QueryInstructionVectorFilter& o)
	:m_impl(o.m_impl),m_ownership(o.m_ownership),m_state(o.m_state),m_index(o.m_index){}

QueryInstructionVectorFilter::QueryInstructionVectorFilter( const std::vector<analyzer::Query::Instruction>* impl)
	:m_impl(impl),m_ownership(0),m_state(1),m_index(0){}

QueryInstructionVectorFilter::QueryInstructionVectorFilter( std::vector<analyzer::Query::Instruction>* impl, bool withOwnership)
	:m_impl(impl),m_ownership(impl),m_state(1),m_index(0){}


BindingFilterInterface::Tag QueryInstructionVectorFilter::getNext( bindings::ValueVariant& val)
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

void QueryInstructionVectorFilter::skip()
{
	const filter::StateTable::Element& st = g_array_statetable[ m_state];
	m_state = st.skipState;
	if (m_state == StateArrayIndex && m_impl && m_index < m_impl->size())
	{
		++m_index;
	}
}

BindingFilterInterface* QueryInstructionVectorFilter::createCopy() const
{
	return new QueryInstructionVectorFilter(*this);
}
