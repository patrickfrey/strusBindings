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
/// \file queryFilter.cpp
#include "queryFilter.hpp"
#include "structElementArray.hpp"
#include "stateTable.hpp"
#include <cstring>

using namespace strus;

static const char* g_element_names[] = { "metadata","name","value","searchTerms","type","value","pos","len","elements","type","idx","position","length","fieldNo","instructions","opCode","idx","nofOperands", 0};
static const filter::StructElementArray g_struct_elements( g_element_names);

enum TermState {
	StateEnd,
	StateMetadataOpen,
	StateMetadataIndex,
	StateMetadataNameOpen,
	StateMetadataNameValue,
	StateMetadataNameClose,
	StateMetadataValueOpen,
	StateMetadataValueValue,
	StateMetadataValueClose,
	StateMetadataClose,
	StateSearchTermsOpen,
	StateSearchTermsIndex,
	StateSearchTermsTypeOpen,
	StateSearchTermsTypeValue,
	StateSearchTermsTypeClose,
	StateSearchTermsValueOpen,
	StateSearchTermsValueValue,
	StateSearchTermsValueClose,
	StateSearchTermsPosOpen,
	StateSearchTermsPosValue,
	StateSearchTermsPosClose,
	StateSearchTermsLenOpen,
	StateSearchTermsLenValue,
	StateSearchTermsLenClose,
	StateSearchTermsClose,
	StateElementsOpen,
	StateElementsIndex,
	StateElementsTypeOpen,
	StateElementsTypeValue,
	StateElementsTypeClose,
	StateElementsIdxOpen,
	StateElementsIdxValue,
	StateElementsIdxClose,
	StateElementsPositionOpen,
	StateElementsPositionValue,
	StateElementsPositionClose,
	StateElementsLengthOpen,
	StateElementsLengthValue,
	StateElementsLengthClose,
	StateElementsFieldNoOpen,
	StateElementsFieldNoValue,
	StateElementsFieldNoClose,
	StateElementsClose,
	StateInstructionsOpen,
	StateInstructionsIndex,
	StateInstructionsOpCodeOpen,
	StateInstructionsOpCodeValue,
	StateInstructionsOpCodeClose,
	StateInstructionsIdxOpen,
	StateInstructionsIdxValue,
	StateInstructionsIdxClose,
	StateInstructionsNofOperandsOpen,
	StateInstructionsNofOperandsValue,
	StateInstructionsNofOperandsClose,
	StateInstructionsClose
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
	{StateMetadataOpen, _OPEN, StateMetadataIndex, StateSearchTermsOpen, _TAG, 0, -1},
	{StateMetadataIndex, _INDEX, StateMetadataNameOpen, StateMetadataIndex, _TAG, -1, -1},
	{StateMetadataNameOpen, _OPEN, StateMetadataNameValue, StateMetadataValueOpen, _TAG, 1, -1},
	{StateMetadataNameValue, _VALUE, StateMetadataNameClose, StateMetadataNameClose, _ELEM, -1, 0},
	{StateMetadataNameClose, _CLOSE, StateMetadataValueOpen, StateMetadataValueOpen, _NULL, -1, -1},
	{StateMetadataValueOpen, _OPEN, StateMetadataValueValue, StateMetadataIndex, _TAG, 2, -1},
	{StateMetadataValueValue, _VALUE, StateMetadataValueClose, StateMetadataValueClose, _ELEM, -1, 1},
	{StateMetadataValueClose, _CLOSE, StateMetadataIndex, StateMetadataIndex, _NULL, -1, -1},
	{StateMetadataClose, _CLOSE, StateSearchTermsOpen, StateSearchTermsOpen, _NULL, -1, -1},
	{StateSearchTermsOpen, _OPEN, StateSearchTermsIndex, StateElementsOpen, _TAG, 3, -1},
	{StateSearchTermsIndex, _INDEX, StateSearchTermsTypeOpen, StateSearchTermsIndex, _TAG, -1, -1},
	{StateSearchTermsTypeOpen, _OPEN, StateSearchTermsTypeValue, StateSearchTermsValueOpen, _TAG, 4, -1},
	{StateSearchTermsTypeValue, _VALUE, StateSearchTermsTypeClose, StateSearchTermsTypeClose, _ELEM, -1, 0},
	{StateSearchTermsTypeClose, _CLOSE, StateSearchTermsValueOpen, StateSearchTermsValueOpen, _NULL, -1, -1},
	{StateSearchTermsValueOpen, _OPEN, StateSearchTermsValueValue, StateSearchTermsPosOpen, _TAG, 5, -1},
	{StateSearchTermsValueValue, _VALUE, StateSearchTermsValueClose, StateSearchTermsValueClose, _ELEM, -1, 1},
	{StateSearchTermsValueClose, _CLOSE, StateSearchTermsPosOpen, StateSearchTermsPosOpen, _NULL, -1, -1},
	{StateSearchTermsPosOpen, _OPEN, StateSearchTermsPosValue, StateSearchTermsLenOpen, _TAG, 6, -1},
	{StateSearchTermsPosValue, _VALUE, StateSearchTermsPosClose, StateSearchTermsPosClose, _ELEM, -1, 2},
	{StateSearchTermsPosClose, _CLOSE, StateSearchTermsLenOpen, StateSearchTermsLenOpen, _NULL, -1, -1},
	{StateSearchTermsLenOpen, _OPEN, StateSearchTermsLenValue, StateSearchTermsIndex, _TAG, 7, -1},
	{StateSearchTermsLenValue, _VALUE, StateSearchTermsLenClose, StateSearchTermsLenClose, _ELEM, -1, 3},
	{StateSearchTermsLenClose, _CLOSE, StateSearchTermsIndex, StateSearchTermsIndex, _NULL, -1, -1},
	{StateSearchTermsClose, _CLOSE, StateElementsOpen, StateElementsOpen, _NULL, -1, -1},
	{StateElementsOpen, _OPEN, StateElementsIndex, StateInstructionsOpen, _TAG, 8, -1},
	{StateElementsIndex, _INDEX, StateElementsTypeOpen, StateElementsIndex, _TAG, -1, -1},
	{StateElementsTypeOpen, _OPEN, StateElementsTypeValue, StateElementsIdxOpen, _TAG, 9, -1},
	{StateElementsTypeValue, _VALUE, StateElementsTypeClose, StateElementsTypeClose, _ELEM, -1, 0},
	{StateElementsTypeClose, _CLOSE, StateElementsIdxOpen, StateElementsIdxOpen, _NULL, -1, -1},
	{StateElementsIdxOpen, _OPEN, StateElementsIdxValue, StateElementsPositionOpen, _TAG, 10, -1},
	{StateElementsIdxValue, _VALUE, StateElementsIdxClose, StateElementsIdxClose, _ELEM, -1, 1},
	{StateElementsIdxClose, _CLOSE, StateElementsPositionOpen, StateElementsPositionOpen, _NULL, -1, -1},
	{StateElementsPositionOpen, _OPEN, StateElementsPositionValue, StateElementsLengthOpen, _TAG, 11, -1},
	{StateElementsPositionValue, _VALUE, StateElementsPositionClose, StateElementsPositionClose, _ELEM, -1, 2},
	{StateElementsPositionClose, _CLOSE, StateElementsLengthOpen, StateElementsLengthOpen, _NULL, -1, -1},
	{StateElementsLengthOpen, _OPEN, StateElementsLengthValue, StateElementsFieldNoOpen, _TAG, 12, -1},
	{StateElementsLengthValue, _VALUE, StateElementsLengthClose, StateElementsLengthClose, _ELEM, -1, 3},
	{StateElementsLengthClose, _CLOSE, StateElementsFieldNoOpen, StateElementsFieldNoOpen, _NULL, -1, -1},
	{StateElementsFieldNoOpen, _OPEN, StateElementsFieldNoValue, StateElementsIndex, _TAG, 13, -1},
	{StateElementsFieldNoValue, _VALUE, StateElementsFieldNoClose, StateElementsFieldNoClose, _ELEM, -1, 4},
	{StateElementsFieldNoClose, _CLOSE, StateElementsIndex, StateElementsIndex, _NULL, -1, -1},
	{StateElementsClose, _CLOSE, StateInstructionsOpen, StateInstructionsOpen, _NULL, -1, -1},
	{StateInstructionsOpen, _OPEN, StateInstructionsIndex, StateEnd, _TAG, 14, -1},
	{StateInstructionsIndex, _INDEX, StateInstructionsOpCodeOpen, StateInstructionsIndex, _TAG, -1, -1},
	{StateInstructionsOpCodeOpen, _OPEN, StateInstructionsOpCodeValue, StateInstructionsIdxOpen, _TAG, 15, -1},
	{StateInstructionsOpCodeValue, _VALUE, StateInstructionsOpCodeClose, StateInstructionsOpCodeClose, _ELEM, -1, 0},
	{StateInstructionsOpCodeClose, _CLOSE, StateInstructionsIdxOpen, StateInstructionsIdxOpen, _NULL, -1, -1},
	{StateInstructionsIdxOpen, _OPEN, StateInstructionsIdxValue, StateInstructionsNofOperandsOpen, _TAG, 16, -1},
	{StateInstructionsIdxValue, _VALUE, StateInstructionsIdxClose, StateInstructionsIdxClose, _ELEM, -1, 1},
	{StateInstructionsIdxClose, _CLOSE, StateInstructionsNofOperandsOpen, StateInstructionsNofOperandsOpen, _NULL, -1, -1},
	{StateInstructionsNofOperandsOpen, _OPEN, StateInstructionsNofOperandsValue, StateInstructionsIndex, _TAG, 17, -1},
	{StateInstructionsNofOperandsValue, _VALUE, StateInstructionsNofOperandsClose, StateInstructionsNofOperandsClose, _ELEM, -1, 2},
	{StateInstructionsNofOperandsClose, _CLOSE, StateInstructionsIndex, StateInstructionsIndex, _NULL, -1, -1},
	{StateInstructionsClose, _CLOSE, StateEnd, StateEnd, _NULL, -1, -1}
};

QueryFilter::QueryFilter()
	:m_impl(0),m_ownership(0),m_state(0)
{
	std::memset( m_index, 0, sizeof(m_index));
}

QueryFilter::QueryFilter( const QueryFilter& o)
	:m_impl(o.m_impl),m_ownership(o.m_ownership),m_state(o.m_state)
{
	std::memcpy( m_index, o.m_index, sizeof(m_index));
}

QueryFilter::QueryFilter( const analyzer::Query* impl)
	:m_impl(impl),m_ownership(0),m_state(1)
{
	std::memset( m_index, 0, sizeof(m_index));
}

QueryFilter::QueryFilter( analyzer::Query* impl, bool withOwnership)
	:m_impl(impl),m_ownership(impl),m_state(1)
{
	std::memset( m_index, 0, sizeof(m_index));
}

QueryFilter::~QueryFilter()
{
	if (m_ownership) delete( m_ownership);
}

static bindings::ValueVariant getElementValue( const analyzer::Query& elem, int valueIndex)
{
	switch (valueIndex) {

		case 0:
			return bindings::ValueVariant( (*m_impl).metadata()[m_index[0]].name());

		case 1:
			return bindings::ValueVariant( (*m_impl).metadata()[m_index[0]].value());

		case 2:
			return bindings::ValueVariant( (*m_impl).searchTerms()[m_index[0]].type());

		case 3:
			return bindings::ValueVariant( (*m_impl).searchTerms()[m_index[0]].value());

		case 4:
			return bindings::ValueVariant( (bindings::ValueVariant::UIntType)(*m_impl).searchTerms()[m_index[0]].pos());

		case 5:
			return bindings::ValueVariant( (bindings::ValueVariant::UIntType)(*m_impl).searchTerms()[m_index[0]].len());

		case 6:
			return bindings::ValueVariant( analyzer::Query::Element::typeName( (*m_impl).elements()[m_index[0]].type()));

		case 7:
			return bindings::ValueVariant( (bindings::ValueVariant::UIntType)(*m_impl).elements()[m_index[0]].idx());

		case 8:
			return bindings::ValueVariant( (bindings::ValueVariant::UIntType)(*m_impl).elements()[m_index[0]].position());

		case 9:
			return bindings::ValueVariant( (bindings::ValueVariant::UIntType)(*m_impl).elements()[m_index[0]].length());

		case 10:
			return bindings::ValueVariant( (bindings::ValueVariant::UIntType)(*m_impl).elements()[m_index[0]].fieldNo());

		case 11:
			return bindings::ValueVariant( analyzer::Query::Instruction::opCodeName( (*m_impl).instructions()[m_index[0]].opCode()));

		case 12:
			return bindings::ValueVariant( (bindings::ValueVariant::UIntType)(*m_impl).instructions()[m_index[0]].idx());

		case 13:
			return bindings::ValueVariant( (bindings::ValueVariant::UIntType)(*m_impl).instructions()[m_index[0]].nofOperands());

	}
	return bindings::ValueVariant();
}

BindingFilterInterface::Tag QueryFilter::getNext( bindings::ValueVariant& val)
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

void QueryFilter::skip()
{
	const filter::StateTable::Element& st = g_struct_statetable[ m_state];
	m_state = st.skipState;
}

BindingFilterInterface* QueryFilter::createCopy() const
{
	return new QueryFilter(*this);
}

