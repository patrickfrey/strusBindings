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
/// \file documentFilter.cpp
#include "documentFilter.hpp"
#include "structElementArray.hpp"
#include "stateTable.hpp"
#include "variantValueTemplate.hpp"
#include <cstring>

using namespace strus;

static const char* g_element_names[] = { "subDocumentTypeName","metadata","name","value","attributes","name","value","searchTerms","type","value","pos","len","forwardTerms","type","value","pos","len", 0};
static const filter::StructElementArray g_struct_elements( g_element_names);

enum TermState {
	StateEnd,
	StateSubDocumentTypeNameOpen,
	StateSubDocumentTypeNameValue,
	StateSubDocumentTypeNameClose,
	StateMetadataOpen,
	StateMetadataIndex,
	StateMetadataNameOpen,
	StateMetadataNameValue,
	StateMetadataNameClose,
	StateMetadataValueOpen,
	StateMetadataValueValue,
	StateMetadataValueClose,
	StateMetadataClose,
	StateAttributesOpen,
	StateAttributesIndex,
	StateAttributesNameOpen,
	StateAttributesNameValue,
	StateAttributesNameClose,
	StateAttributesValueOpen,
	StateAttributesValueValue,
	StateAttributesValueClose,
	StateAttributesClose,
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
	StateForwardTermsOpen,
	StateForwardTermsIndex,
	StateForwardTermsTypeOpen,
	StateForwardTermsTypeValue,
	StateForwardTermsTypeClose,
	StateForwardTermsValueOpen,
	StateForwardTermsValueValue,
	StateForwardTermsValueClose,
	StateForwardTermsPosOpen,
	StateForwardTermsPosValue,
	StateForwardTermsPosClose,
	StateForwardTermsLenOpen,
	StateForwardTermsLenValue,
	StateForwardTermsLenClose,
	StateForwardTermsClose
};

enum TermArrayState {
	StateArrayEnd,
	StateIndex,
	StateSubDocumentTypeNameOpen,
	StateSubDocumentTypeNameValue,
	StateSubDocumentTypeNameClose,
	StateMetadataOpen,
	StateMetadataIndex,
	StateMetadataNameOpen,
	StateMetadataNameValue,
	StateMetadataNameClose,
	StateMetadataValueOpen,
	StateMetadataValueValue,
	StateMetadataValueClose,
	StateMetadataClose,
	StateAttributesOpen,
	StateAttributesIndex,
	StateAttributesNameOpen,
	StateAttributesNameValue,
	StateAttributesNameClose,
	StateAttributesValueOpen,
	StateAttributesValueValue,
	StateAttributesValueClose,
	StateAttributesClose,
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
	StateForwardTermsOpen,
	StateForwardTermsIndex,
	StateForwardTermsTypeOpen,
	StateForwardTermsTypeValue,
	StateForwardTermsTypeClose,
	StateForwardTermsValueOpen,
	StateForwardTermsValueValue,
	StateForwardTermsValueClose,
	StateForwardTermsPosOpen,
	StateForwardTermsPosValue,
	StateForwardTermsPosClose,
	StateForwardTermsLenOpen,
	StateForwardTermsLenValue,
	StateForwardTermsLenClose,
	StateForwardTermsClose
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
	{StateSubDocumentTypeNameOpen, _OPEN, StateSubDocumentTypeNameValue, StateMetadataOpen, _TAG, 0, -1},
	{StateSubDocumentTypeNameValue, _VALUE, StateSubDocumentTypeNameClose, StateSubDocumentTypeNameClose, _ELEM, -1, 0},
	{StateSubDocumentTypeNameClose, _CLOSE, StateMetadataOpen, StateMetadataOpen, _NULL, -1, -1},
	{StateMetadataOpen, _OPEN, StateMetadataIndex, StateAttributesOpen, _TAG, 1, -1},
	{StateMetadataIndex, _INDEX, StateMetadataNameOpen, StateMetadataIndex, _TAG, -1, -1},
	{StateMetadataNameOpen, _OPEN, StateMetadataNameValue, StateMetadataValueOpen, _TAG, 2, -1},
	{StateMetadataNameValue, _VALUE, StateMetadataNameClose, StateMetadataNameClose, _ELEM, -1, 0},
	{StateMetadataNameClose, _CLOSE, StateMetadataValueOpen, StateMetadataValueOpen, _NULL, -1, -1},
	{StateMetadataValueOpen, _OPEN, StateMetadataValueValue, StateMetadataIndex, _TAG, 3, -1},
	{StateMetadataValueValue, _VALUE, StateMetadataValueClose, StateMetadataValueClose, _ELEM, -1, 1},
	{StateMetadataValueClose, _CLOSE, StateMetadataIndex, StateMetadataIndex, _NULL, -1, -1},
	{StateMetadataClose, _CLOSE, StateAttributesOpen, StateAttributesOpen, _NULL, -1, -1},
	{StateAttributesOpen, _OPEN, StateAttributesIndex, StateSearchTermsOpen, _TAG, 4, -1},
	{StateAttributesIndex, _INDEX, StateAttributesNameOpen, StateAttributesIndex, _TAG, -1, -1},
	{StateAttributesNameOpen, _OPEN, StateAttributesNameValue, StateAttributesValueOpen, _TAG, 5, -1},
	{StateAttributesNameValue, _VALUE, StateAttributesNameClose, StateAttributesNameClose, _ELEM, -1, 0},
	{StateAttributesNameClose, _CLOSE, StateAttributesValueOpen, StateAttributesValueOpen, _NULL, -1, -1},
	{StateAttributesValueOpen, _OPEN, StateAttributesValueValue, StateAttributesIndex, _TAG, 6, -1},
	{StateAttributesValueValue, _VALUE, StateAttributesValueClose, StateAttributesValueClose, _ELEM, -1, 1},
	{StateAttributesValueClose, _CLOSE, StateAttributesIndex, StateAttributesIndex, _NULL, -1, -1},
	{StateAttributesClose, _CLOSE, StateSearchTermsOpen, StateSearchTermsOpen, _NULL, -1, -1},
	{StateSearchTermsOpen, _OPEN, StateSearchTermsIndex, StateForwardTermsOpen, _TAG, 7, -1},
	{StateSearchTermsIndex, _INDEX, StateSearchTermsTypeOpen, StateSearchTermsIndex, _TAG, -1, -1},
	{StateSearchTermsTypeOpen, _OPEN, StateSearchTermsTypeValue, StateSearchTermsValueOpen, _TAG, 8, -1},
	{StateSearchTermsTypeValue, _VALUE, StateSearchTermsTypeClose, StateSearchTermsTypeClose, _ELEM, -1, 0},
	{StateSearchTermsTypeClose, _CLOSE, StateSearchTermsValueOpen, StateSearchTermsValueOpen, _NULL, -1, -1},
	{StateSearchTermsValueOpen, _OPEN, StateSearchTermsValueValue, StateSearchTermsPosOpen, _TAG, 9, -1},
	{StateSearchTermsValueValue, _VALUE, StateSearchTermsValueClose, StateSearchTermsValueClose, _ELEM, -1, 1},
	{StateSearchTermsValueClose, _CLOSE, StateSearchTermsPosOpen, StateSearchTermsPosOpen, _NULL, -1, -1},
	{StateSearchTermsPosOpen, _OPEN, StateSearchTermsPosValue, StateSearchTermsLenOpen, _TAG, 10, -1},
	{StateSearchTermsPosValue, _VALUE, StateSearchTermsPosClose, StateSearchTermsPosClose, _ELEM, -1, 2},
	{StateSearchTermsPosClose, _CLOSE, StateSearchTermsLenOpen, StateSearchTermsLenOpen, _NULL, -1, -1},
	{StateSearchTermsLenOpen, _OPEN, StateSearchTermsLenValue, StateSearchTermsIndex, _TAG, 11, -1},
	{StateSearchTermsLenValue, _VALUE, StateSearchTermsLenClose, StateSearchTermsLenClose, _ELEM, -1, 3},
	{StateSearchTermsLenClose, _CLOSE, StateSearchTermsIndex, StateSearchTermsIndex, _NULL, -1, -1},
	{StateSearchTermsClose, _CLOSE, StateForwardTermsOpen, StateForwardTermsOpen, _NULL, -1, -1},
	{StateForwardTermsOpen, _OPEN, StateForwardTermsIndex, StateEnd, _TAG, 12, -1},
	{StateForwardTermsIndex, _INDEX, StateForwardTermsTypeOpen, StateForwardTermsIndex, _TAG, -1, -1},
	{StateForwardTermsTypeOpen, _OPEN, StateForwardTermsTypeValue, StateForwardTermsValueOpen, _TAG, 13, -1},
	{StateForwardTermsTypeValue, _VALUE, StateForwardTermsTypeClose, StateForwardTermsTypeClose, _ELEM, -1, 0},
	{StateForwardTermsTypeClose, _CLOSE, StateForwardTermsValueOpen, StateForwardTermsValueOpen, _NULL, -1, -1},
	{StateForwardTermsValueOpen, _OPEN, StateForwardTermsValueValue, StateForwardTermsPosOpen, _TAG, 14, -1},
	{StateForwardTermsValueValue, _VALUE, StateForwardTermsValueClose, StateForwardTermsValueClose, _ELEM, -1, 1},
	{StateForwardTermsValueClose, _CLOSE, StateForwardTermsPosOpen, StateForwardTermsPosOpen, _NULL, -1, -1},
	{StateForwardTermsPosOpen, _OPEN, StateForwardTermsPosValue, StateForwardTermsLenOpen, _TAG, 15, -1},
	{StateForwardTermsPosValue, _VALUE, StateForwardTermsPosClose, StateForwardTermsPosClose, _ELEM, -1, 2},
	{StateForwardTermsPosClose, _CLOSE, StateForwardTermsLenOpen, StateForwardTermsLenOpen, _NULL, -1, -1},
	{StateForwardTermsLenOpen, _OPEN, StateForwardTermsLenValue, StateForwardTermsIndex, _TAG, 16, -1},
	{StateForwardTermsLenValue, _VALUE, StateForwardTermsLenClose, StateForwardTermsLenClose, _ELEM, -1, 3},
	{StateForwardTermsLenClose, _CLOSE, StateForwardTermsIndex, StateForwardTermsIndex, _NULL, -1, -1},
	{StateForwardTermsClose, _CLOSE, StateEnd, StateEnd, _NULL, -1, -1}
};

static const filter::StateTable::Element g_array_statetable[] = {
	{StateArrayEnd, _CLOSE, StateArrayEnd, StateArrayEnd, _NULL, 0, 0},
	{StateArrayIndex, _INDEX, StateArraySubDocumentTypeNameOpen, StateArrayIndex, _TAG, 1, 0},
	{StateArraySubDocumentTypeNameOpen, _OPEN, StateArraySubDocumentTypeNameValue, StateArrayMetadataOpen, _TAG, 0, -1},
	{StateArraySubDocumentTypeNameValue, _VALUE, StateArraySubDocumentTypeNameClose, StateArraySubDocumentTypeNameClose, _ELEM, -1, 0},
	{StateArraySubDocumentTypeNameClose, _CLOSE, StateArrayMetadataOpen, StateArrayMetadataOpen, _NULL, -1, -1},
	{StateArrayMetadataOpen, _OPEN, StateArrayMetadataIndex, StateArrayAttributesOpen, _TAG, 1, -1},
	{StateArrayMetadataIndex, _INDEX, StateArrayMetadataNameOpen, StateArrayMetadataIndex, _TAG, -1, -1},
	{StateArrayMetadataNameOpen, _OPEN, StateArrayMetadataNameValue, StateArrayMetadataValueOpen, _TAG, 2, -1},
	{StateArrayMetadataNameValue, _VALUE, StateArrayMetadataNameClose, StateArrayMetadataNameClose, _ELEM, -1, 0},
	{StateArrayMetadataNameClose, _CLOSE, StateArrayMetadataValueOpen, StateArrayMetadataValueOpen, _NULL, -1, -1},
	{StateArrayMetadataValueOpen, _OPEN, StateArrayMetadataValueValue, StateArrayMetadataIndex, _TAG, 3, -1},
	{StateArrayMetadataValueValue, _VALUE, StateArrayMetadataValueClose, StateArrayMetadataValueClose, _ELEM, -1, 1},
	{StateArrayMetadataValueClose, _CLOSE, StateArrayMetadataIndex, StateArrayMetadataIndex, _NULL, -1, -1},
	{StateArrayMetadataClose, _CLOSE, StateArrayAttributesOpen, StateArrayAttributesOpen, _NULL, -1, -1},
	{StateArrayAttributesOpen, _OPEN, StateArrayAttributesIndex, StateArraySearchTermsOpen, _TAG, 4, -1},
	{StateArrayAttributesIndex, _INDEX, StateArrayAttributesNameOpen, StateArrayAttributesIndex, _TAG, -1, -1},
	{StateArrayAttributesNameOpen, _OPEN, StateArrayAttributesNameValue, StateArrayAttributesValueOpen, _TAG, 5, -1},
	{StateArrayAttributesNameValue, _VALUE, StateArrayAttributesNameClose, StateArrayAttributesNameClose, _ELEM, -1, 0},
	{StateArrayAttributesNameClose, _CLOSE, StateArrayAttributesValueOpen, StateArrayAttributesValueOpen, _NULL, -1, -1},
	{StateArrayAttributesValueOpen, _OPEN, StateArrayAttributesValueValue, StateArrayAttributesIndex, _TAG, 6, -1},
	{StateArrayAttributesValueValue, _VALUE, StateArrayAttributesValueClose, StateArrayAttributesValueClose, _ELEM, -1, 1},
	{StateArrayAttributesValueClose, _CLOSE, StateArrayAttributesIndex, StateArrayAttributesIndex, _NULL, -1, -1},
	{StateArrayAttributesClose, _CLOSE, StateArraySearchTermsOpen, StateArraySearchTermsOpen, _NULL, -1, -1},
	{StateArraySearchTermsOpen, _OPEN, StateArraySearchTermsIndex, StateArrayForwardTermsOpen, _TAG, 7, -1},
	{StateArraySearchTermsIndex, _INDEX, StateArraySearchTermsTypeOpen, StateArraySearchTermsIndex, _TAG, -1, -1},
	{StateArraySearchTermsTypeOpen, _OPEN, StateArraySearchTermsTypeValue, StateArraySearchTermsValueOpen, _TAG, 8, -1},
	{StateArraySearchTermsTypeValue, _VALUE, StateArraySearchTermsTypeClose, StateArraySearchTermsTypeClose, _ELEM, -1, 0},
	{StateArraySearchTermsTypeClose, _CLOSE, StateArraySearchTermsValueOpen, StateArraySearchTermsValueOpen, _NULL, -1, -1},
	{StateArraySearchTermsValueOpen, _OPEN, StateArraySearchTermsValueValue, StateArraySearchTermsPosOpen, _TAG, 9, -1},
	{StateArraySearchTermsValueValue, _VALUE, StateArraySearchTermsValueClose, StateArraySearchTermsValueClose, _ELEM, -1, 1},
	{StateArraySearchTermsValueClose, _CLOSE, StateArraySearchTermsPosOpen, StateArraySearchTermsPosOpen, _NULL, -1, -1},
	{StateArraySearchTermsPosOpen, _OPEN, StateArraySearchTermsPosValue, StateArraySearchTermsLenOpen, _TAG, 10, -1},
	{StateArraySearchTermsPosValue, _VALUE, StateArraySearchTermsPosClose, StateArraySearchTermsPosClose, _ELEM, -1, 2},
	{StateArraySearchTermsPosClose, _CLOSE, StateArraySearchTermsLenOpen, StateArraySearchTermsLenOpen, _NULL, -1, -1},
	{StateArraySearchTermsLenOpen, _OPEN, StateArraySearchTermsLenValue, StateArraySearchTermsIndex, _TAG, 11, -1},
	{StateArraySearchTermsLenValue, _VALUE, StateArraySearchTermsLenClose, StateArraySearchTermsLenClose, _ELEM, -1, 3},
	{StateArraySearchTermsLenClose, _CLOSE, StateArraySearchTermsIndex, StateArraySearchTermsIndex, _NULL, -1, -1},
	{StateArraySearchTermsClose, _CLOSE, StateArrayForwardTermsOpen, StateArrayForwardTermsOpen, _NULL, -1, -1},
	{StateArrayForwardTermsOpen, _OPEN, StateArrayForwardTermsIndex, StateArrayIndex, _TAG, 12, -1},
	{StateArrayForwardTermsIndex, _INDEX, StateArrayForwardTermsTypeOpen, StateArrayForwardTermsIndex, _TAG, -1, -1},
	{StateArrayForwardTermsTypeOpen, _OPEN, StateArrayForwardTermsTypeValue, StateArrayForwardTermsValueOpen, _TAG, 13, -1},
	{StateArrayForwardTermsTypeValue, _VALUE, StateArrayForwardTermsTypeClose, StateArrayForwardTermsTypeClose, _ELEM, -1, 0},
	{StateArrayForwardTermsTypeClose, _CLOSE, StateArrayForwardTermsValueOpen, StateArrayForwardTermsValueOpen, _NULL, -1, -1},
	{StateArrayForwardTermsValueOpen, _OPEN, StateArrayForwardTermsValueValue, StateArrayForwardTermsPosOpen, _TAG, 14, -1},
	{StateArrayForwardTermsValueValue, _VALUE, StateArrayForwardTermsValueClose, StateArrayForwardTermsValueClose, _ELEM, -1, 1},
	{StateArrayForwardTermsValueClose, _CLOSE, StateArrayForwardTermsPosOpen, StateArrayForwardTermsPosOpen, _NULL, -1, -1},
	{StateArrayForwardTermsPosOpen, _OPEN, StateArrayForwardTermsPosValue, StateArrayForwardTermsLenOpen, _TAG, 15, -1},
	{StateArrayForwardTermsPosValue, _VALUE, StateArrayForwardTermsPosClose, StateArrayForwardTermsPosClose, _ELEM, -1, 2},
	{StateArrayForwardTermsPosClose, _CLOSE, StateArrayForwardTermsLenOpen, StateArrayForwardTermsLenOpen, _NULL, -1, -1},
	{StateArrayForwardTermsLenOpen, _OPEN, StateArrayForwardTermsLenValue, StateArrayForwardTermsIndex, _TAG, 16, -1},
	{StateArrayForwardTermsLenValue, _VALUE, StateArrayForwardTermsLenClose, StateArrayForwardTermsLenClose, _ELEM, -1, 3},
	{StateArrayForwardTermsLenClose, _CLOSE, StateArrayForwardTermsIndex, StateArrayForwardTermsIndex, _NULL, -1, -1},
	{StateArrayForwardTermsClose, _CLOSE, StateArrayIndex, StateArrayIndex, _NULL, -1, -1}
};

DocumentFilter::DocumentFilter()
	:m_impl(0),m_ownership(0),m_state(0)
{
	std::memset( m_index, 0, sizeof(m_index));
}

DocumentFilter::DocumentFilter( const DocumentFilter& o)
	:m_impl(o.m_impl),m_ownership(o.m_ownership),m_state(o.m_state)
{
	std::memcpy( m_index, o.m_index, sizeof(m_index));
}

DocumentFilter::DocumentFilter( const analyzer::Document* impl)
	:m_impl(impl),m_ownership(0),m_state(1)
{
	std::memset( m_index, 0, sizeof(m_index));
}

DocumentFilter::DocumentFilter( analyzer::Document* impl, bool withOwnership)
	:m_impl(impl),m_ownership(impl),m_state(1)
{
	std::memset( m_index, 0, sizeof(m_index));
}

DocumentFilter::~DocumentFilter()
{
	if (m_ownership) delete( m_ownership);
}

static bindings::ValueVariant getElementValue( const analyzer::Document& elem, int valueIndex)
{
	switch (valueIndex) {

		case 0:
			return bindings::ValueVariant( (*m_impl).subDocumentTypeName());

		case 1:
			return bindings::ValueVariant( (*m_impl).metadata()[m_index[0]].name());

		case 2:
			return bindings::ValueVariant( (*m_impl).metadata()[m_index[0]].value());

		case 3:
			return bindings::ValueVariant( (*m_impl).attributes()[m_index[0]].name());

		case 4:
			return bindings::ValueVariant( (*m_impl).attributes()[m_index[0]].value());

		case 5:
			return bindings::ValueVariant( (*m_impl).searchTerms()[m_index[0]].type());

		case 6:
			return bindings::ValueVariant( (*m_impl).searchTerms()[m_index[0]].value());

		case 7:
			return bindings::ValueVariant( (bindings::ValueVariant::UIntType)(*m_impl).searchTerms()[m_index[0]].pos());

		case 8:
			return bindings::ValueVariant( (bindings::ValueVariant::UIntType)(*m_impl).searchTerms()[m_index[0]].len());

		case 9:
			return bindings::ValueVariant( (*m_impl).forwardTerms()[m_index[0]].type());

		case 10:
			return bindings::ValueVariant( (*m_impl).forwardTerms()[m_index[0]].value());

		case 11:
			return bindings::ValueVariant( (bindings::ValueVariant::UIntType)(*m_impl).forwardTerms()[m_index[0]].pos());

		case 12:
			return bindings::ValueVariant( (bindings::ValueVariant::UIntType)(*m_impl).forwardTerms()[m_index[0]].len());

		}
	return bindings::ValueVariant();
}

BindingFilterInterface::Tag DocumentFilter::getNext( bindings::ValueVariant& val)
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

void DocumentFilter::skip()
{
	const filter::StateTable::Element& st = g_struct_statetable[ m_state];
	m_state = st.skipState;
}

BindingFilterInterface* DocumentFilter::createCopy() const
{
	return new DocumentFilter(*this);
}

