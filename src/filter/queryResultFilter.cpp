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
/// \file queryResultFilter.cpp
#include "queryResultFilter.hpp"
#include "structElementArray.hpp"
#include "stateTable.hpp"
#include <cstring>

using namespace strus;

static const char* g_element_names[] = { "evaluationPass","nofRanked","nofVisited","ranks","docno","weight","summaryElements","name","value","weight","index", 0};
static const filter::StructElementArray g_struct_elements( g_element_names);

enum TermState {
	StateEnd,
	StateEvaluationPassOpen,
	StateEvaluationPassValue,
	StateEvaluationPassClose,
	StateNofRankedOpen,
	StateNofRankedValue,
	StateNofRankedClose,
	StateNofVisitedOpen,
	StateNofVisitedValue,
	StateNofVisitedClose,
	StateRanksOpen,
	StateRanksIndex,
	StateRanksDocnoOpen,
	StateRanksDocnoValue,
	StateRanksDocnoClose,
	StateRanksWeightOpen,
	StateRanksWeightValue,
	StateRanksWeightClose,
	StateRanksSummaryElementsOpen,
	StateRanksSummaryElementsIndex,
	StateRanksSummaryElementsNameOpen,
	StateRanksSummaryElementsNameValue,
	StateRanksSummaryElementsNameClose,
	StateRanksSummaryElementsValueOpen,
	StateRanksSummaryElementsValueValue,
	StateRanksSummaryElementsValueClose,
	StateRanksSummaryElementsWeightOpen,
	StateRanksSummaryElementsWeightValue,
	StateRanksSummaryElementsWeightClose,
	StateRanksSummaryElementsIndexOpen,
	StateRanksSummaryElementsIndexValue,
	StateRanksSummaryElementsIndexClose,
	StateRanksSummaryElementsClose,
	StateRanksClose
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
	{StateEvaluationPassOpen, _OPEN, StateEvaluationPassValue, StateNofRankedOpen, _TAG, 0, -1},
	{StateEvaluationPassValue, _VALUE, StateEvaluationPassClose, StateEvaluationPassClose, _ELEM, -1, 0},
	{StateEvaluationPassClose, _CLOSE, StateNofRankedOpen, StateNofRankedOpen, _NULL, -1, -1},
	{StateNofRankedOpen, _OPEN, StateNofRankedValue, StateNofVisitedOpen, _TAG, 1, -1},
	{StateNofRankedValue, _VALUE, StateNofRankedClose, StateNofRankedClose, _ELEM, -1, 1},
	{StateNofRankedClose, _CLOSE, StateNofVisitedOpen, StateNofVisitedOpen, _NULL, -1, -1},
	{StateNofVisitedOpen, _OPEN, StateNofVisitedValue, StateRanksOpen, _TAG, 2, -1},
	{StateNofVisitedValue, _VALUE, StateNofVisitedClose, StateNofVisitedClose, _ELEM, -1, 2},
	{StateNofVisitedClose, _CLOSE, StateRanksOpen, StateRanksOpen, _NULL, -1, -1},
	{StateRanksOpen, _OPEN, StateRanksIndex, StateEnd, _TAG, 3, -1},
	{StateRanksIndex, _INDEX, StateRanksDocnoOpen, StateRanksIndex, _TAG, -1, -1},
	{StateRanksDocnoOpen, _OPEN, StateRanksDocnoValue, StateRanksWeightOpen, _TAG, 4, -1},
	{StateRanksDocnoValue, _VALUE, StateRanksDocnoClose, StateRanksDocnoClose, _ELEM, -1, 0},
	{StateRanksDocnoClose, _CLOSE, StateRanksWeightOpen, StateRanksWeightOpen, _NULL, -1, -1},
	{StateRanksWeightOpen, _OPEN, StateRanksWeightValue, StateRanksSummaryElementsOpen, _TAG, 5, -1},
	{StateRanksWeightValue, _VALUE, StateRanksWeightClose, StateRanksWeightClose, _ELEM, -1, 1},
	{StateRanksWeightClose, _CLOSE, StateRanksSummaryElementsOpen, StateRanksSummaryElementsOpen, _NULL, -1, -1},
	{StateRanksSummaryElementsOpen, _OPEN, StateRanksSummaryElementsIndex, StateRanksIndex, _TAG, 6, -1},
	{StateRanksSummaryElementsIndex, _INDEX, StateRanksSummaryElementsNameOpen, StateRanksSummaryElementsIndex, _TAG, -1, -1},
	{StateRanksSummaryElementsNameOpen, _OPEN, StateRanksSummaryElementsNameValue, StateRanksSummaryElementsValueOpen, _TAG, 7, -1},
	{StateRanksSummaryElementsNameValue, _VALUE, StateRanksSummaryElementsNameClose, StateRanksSummaryElementsNameClose, _ELEM, -1, 0},
	{StateRanksSummaryElementsNameClose, _CLOSE, StateRanksSummaryElementsValueOpen, StateRanksSummaryElementsValueOpen, _NULL, -1, -1},
	{StateRanksSummaryElementsValueOpen, _OPEN, StateRanksSummaryElementsValueValue, StateRanksSummaryElementsWeightOpen, _TAG, 8, -1},
	{StateRanksSummaryElementsValueValue, _VALUE, StateRanksSummaryElementsValueClose, StateRanksSummaryElementsValueClose, _ELEM, -1, 1},
	{StateRanksSummaryElementsValueClose, _CLOSE, StateRanksSummaryElementsWeightOpen, StateRanksSummaryElementsWeightOpen, _NULL, -1, -1},
	{StateRanksSummaryElementsWeightOpen, _OPEN, StateRanksSummaryElementsWeightValue, StateRanksSummaryElementsIndexOpen, _TAG, 9, -1},
	{StateRanksSummaryElementsWeightValue, _VALUE, StateRanksSummaryElementsWeightClose, StateRanksSummaryElementsWeightClose, _ELEM, -1, 2},
	{StateRanksSummaryElementsWeightClose, _CLOSE, StateRanksSummaryElementsIndexOpen, StateRanksSummaryElementsIndexOpen, _NULL, -1, -1},
	{StateRanksSummaryElementsIndexOpen, _OPEN, StateRanksSummaryElementsIndexValue, StateRanksSummaryElementsIndex, _TAG, 10, -1},
	{StateRanksSummaryElementsIndexValue, _VALUE, StateRanksSummaryElementsIndexClose, StateRanksSummaryElementsIndexClose, _ELEM, -1, 3},
	{StateRanksSummaryElementsIndexClose, _CLOSE, StateRanksSummaryElementsIndex, StateRanksSummaryElementsIndex, _NULL, -1, -1},
	{StateRanksSummaryElementsClose, _CLOSE, StateRanksIndex, StateRanksIndex, _NULL, -1, -1},
	{StateRanksClose, _CLOSE, StateEnd, StateEnd, _NULL, -1, -1}
};

QueryResultFilter::QueryResultFilter()
	:m_impl(0),m_ownership(0),m_state(0)
{
	std::memset( m_index, 0, sizeof(m_index));
}

QueryResultFilter::QueryResultFilter( const QueryResultFilter& o)
	:m_impl(o.m_impl),m_ownership(o.m_ownership),m_state(o.m_state)
{
	std::memcpy( m_index, o.m_index, sizeof(m_index));
}

QueryResultFilter::QueryResultFilter( const QueryResult* impl)
	:m_impl(impl),m_ownership(0),m_state(1)
{
	std::memset( m_index, 0, sizeof(m_index));
}

QueryResultFilter::QueryResultFilter( QueryResult* impl, bool withOwnership)
	:m_impl(impl),m_ownership(impl),m_state(1)
{
	std::memset( m_index, 0, sizeof(m_index));
}

QueryResultFilter::~QueryResultFilter()
{
	if (m_ownership) delete( m_ownership);
}

static bindings::ValueVariant getElementValue( const QueryResult& elem, int valueIndex)
{
	switch (valueIndex) {

		case 0:
			return bindings::ValueVariant( (bindings::ValueVariant::UIntType)(*m_impl).evaluationPass());

		case 1:
			return bindings::ValueVariant( (bindings::ValueVariant::UIntType)(*m_impl).nofRanked());

		case 2:
			return bindings::ValueVariant( (bindings::ValueVariant::UIntType)(*m_impl).nofVisited());

		case 3:
			return bindings::ValueVariant( (bindings::ValueVariant::IntType)(*m_impl).ranks()[m_index[0]].docno());

		case 4:
			return bindings::ValueVariant( (*m_impl).ranks()[m_index[0]].weight());

		case 5:
			return bindings::ValueVariant( (*m_impl).ranks()[m_index[0]].summaryElements()[m_index[1]].name());

		case 6:
			return bindings::ValueVariant( (*m_impl).ranks()[m_index[0]].summaryElements()[m_index[1]].value());

		case 7:
			return bindings::ValueVariant( (*m_impl).ranks()[m_index[0]].summaryElements()[m_index[1]].weight());

		case 8:
			return bindings::ValueVariant( (bindings::ValueVariant::IntType)(*m_impl).ranks()[m_index[0]].summaryElements()[m_index[1]].index());

	}
	return bindings::ValueVariant();
}

BindingFilterInterface::Tag QueryResultFilter::getNext( bindings::ValueVariant& val)
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

void QueryResultFilter::skip()
{
	const filter::StateTable::Element& st = g_struct_statetable[ m_state];
	m_state = st.skipState;
}

BindingFilterInterface* QueryResultFilter::createCopy() const
{
	return new QueryResultFilter(*this);
}

