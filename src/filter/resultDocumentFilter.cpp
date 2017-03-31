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
/// \file resultDocumentFilter.cpp
#include "resultDocumentFilter.hpp"
#include "structElementArray.hpp"
#include "stateTable.hpp"
#include <cstring>

using namespace strus;

static const char* g_element_names[] = { "docno","weight","summaryElements","name","value","weight","index", 0};
static const filter::StructElementArray g_struct_elements( g_element_names);

enum TermState {
	StateEnd,
	StateDocnoOpen,
	StateDocnoValue,
	StateDocnoClose,
	StateWeightOpen,
	StateWeightValue,
	StateWeightClose,
	StateSummaryElementsOpen,
	StateSummaryElementsIndex,
	StateSummaryElementsNameOpen,
	StateSummaryElementsNameValue,
	StateSummaryElementsNameClose,
	StateSummaryElementsValueOpen,
	StateSummaryElementsValueValue,
	StateSummaryElementsValueClose,
	StateSummaryElementsWeightOpen,
	StateSummaryElementsWeightValue,
	StateSummaryElementsWeightClose,
	StateSummaryElementsIndexOpen,
	StateSummaryElementsIndexValue,
	StateSummaryElementsIndexClose,
	StateSummaryElementsClose
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
	{StateDocnoOpen, _OPEN, StateDocnoValue, StateWeightOpen, _TAG, 0, -1},
	{StateDocnoValue, _VALUE, StateDocnoClose, StateDocnoClose, _ELEM, -1, 0},
	{StateDocnoClose, _CLOSE, StateWeightOpen, StateWeightOpen, _NULL, -1, -1},
	{StateWeightOpen, _OPEN, StateWeightValue, StateSummaryElementsOpen, _TAG, 1, -1},
	{StateWeightValue, _VALUE, StateWeightClose, StateWeightClose, _ELEM, -1, 1},
	{StateWeightClose, _CLOSE, StateSummaryElementsOpen, StateSummaryElementsOpen, _NULL, -1, -1},
	{StateSummaryElementsOpen, _OPEN, StateSummaryElementsIndex, StateEnd, _TAG, 2, -1},
	{StateSummaryElementsIndex, _INDEX, StateSummaryElementsNameOpen, StateSummaryElementsIndex, _TAG, -1, -1},
	{StateSummaryElementsNameOpen, _OPEN, StateSummaryElementsNameValue, StateSummaryElementsValueOpen, _TAG, 3, -1},
	{StateSummaryElementsNameValue, _VALUE, StateSummaryElementsNameClose, StateSummaryElementsNameClose, _ELEM, -1, 0},
	{StateSummaryElementsNameClose, _CLOSE, StateSummaryElementsValueOpen, StateSummaryElementsValueOpen, _NULL, -1, -1},
	{StateSummaryElementsValueOpen, _OPEN, StateSummaryElementsValueValue, StateSummaryElementsWeightOpen, _TAG, 4, -1},
	{StateSummaryElementsValueValue, _VALUE, StateSummaryElementsValueClose, StateSummaryElementsValueClose, _ELEM, -1, 1},
	{StateSummaryElementsValueClose, _CLOSE, StateSummaryElementsWeightOpen, StateSummaryElementsWeightOpen, _NULL, -1, -1},
	{StateSummaryElementsWeightOpen, _OPEN, StateSummaryElementsWeightValue, StateSummaryElementsIndexOpen, _TAG, 5, -1},
	{StateSummaryElementsWeightValue, _VALUE, StateSummaryElementsWeightClose, StateSummaryElementsWeightClose, _ELEM, -1, 2},
	{StateSummaryElementsWeightClose, _CLOSE, StateSummaryElementsIndexOpen, StateSummaryElementsIndexOpen, _NULL, -1, -1},
	{StateSummaryElementsIndexOpen, _OPEN, StateSummaryElementsIndexValue, StateSummaryElementsIndex, _TAG, 6, -1},
	{StateSummaryElementsIndexValue, _VALUE, StateSummaryElementsIndexClose, StateSummaryElementsIndexClose, _ELEM, -1, 3},
	{StateSummaryElementsIndexClose, _CLOSE, StateSummaryElementsIndex, StateSummaryElementsIndex, _NULL, -1, -1},
	{StateSummaryElementsClose, _CLOSE, StateEnd, StateEnd, _NULL, -1, -1}
};

ResultDocumentFilter::ResultDocumentFilter()
	:m_impl(0),m_ownership(0),m_state(0)
{
	std::memset( m_index, 0, sizeof(m_index));
}

ResultDocumentFilter::ResultDocumentFilter( const ResultDocumentFilter& o)
	:m_impl(o.m_impl),m_ownership(o.m_ownership),m_state(o.m_state)
{
	std::memcpy( m_index, o.m_index, sizeof(m_index));
}

ResultDocumentFilter::ResultDocumentFilter( const ResultDocument* impl)
	:m_impl(impl),m_ownership(0),m_state(1)
{
	std::memset( m_index, 0, sizeof(m_index));
}

ResultDocumentFilter::ResultDocumentFilter( ResultDocument* impl, bool withOwnership)
	:m_impl(impl),m_ownership(impl),m_state(1)
{
	std::memset( m_index, 0, sizeof(m_index));
}

ResultDocumentFilter::~ResultDocumentFilter()
{
	if (m_ownership) delete( m_ownership);
}

static bindings::ValueVariant getElementValue( const ResultDocument& elem, int valueIndex)
{
	switch (valueIndex) {

		case 0:
			return bindings::ValueVariant( (bindings::ValueVariant::IntType)(*m_impl).docno());

		case 1:
			return bindings::ValueVariant( (*m_impl).weight());

		case 2:
			return bindings::ValueVariant( (*m_impl).summaryElements()[m_index[0]].name());

		case 3:
			return bindings::ValueVariant( (*m_impl).summaryElements()[m_index[0]].value());

		case 4:
			return bindings::ValueVariant( (*m_impl).summaryElements()[m_index[0]].weight());

		case 5:
			return bindings::ValueVariant( (bindings::ValueVariant::IntType)(*m_impl).summaryElements()[m_index[0]].index());

	}
	return bindings::ValueVariant();
}

BindingFilterInterface::Tag ResultDocumentFilter::getNext( bindings::ValueVariant& val)
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

void ResultDocumentFilter::skip()
{
	const filter::StateTable::Element& st = g_struct_statetable[ m_state];
	m_state = st.skipState;
}

BindingFilterInterface* ResultDocumentFilter::createCopy() const
{
	return new ResultDocumentFilter(*this);
}

