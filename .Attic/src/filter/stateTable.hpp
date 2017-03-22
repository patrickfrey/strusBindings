/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_FILTER_STATETABLE_HPP_INCLUDED
#define _STRUS_BINDING_FILTER_STATETABLE_HPP_INCLUDED
/// \brief Array of structure elements
/// \file structElementArray.hpp
#include "strus/binding/valueVariant.hpp"
#include "strus/bindingFilterInterface.hpp"
#include <vector>

/// \brief strus toplevel namespace
namespace strus {
namespace filter {

/// \brief Static array of tag names as value variants
class StateTable
{
public:
	typedef BindingFilterInterface::Tag Tag;
	enum ValueType
	{
		NullValue,
		TagValue,
		ElementValue
	};

	struct Element
	{
		int index;
		Tag tag;
		int nextState;
		int skipState;
		ValueType valueType;
		int tableIndex;
		int valueIndex;
	};

	explicit StateTable( const Element* elements_)
		:m_ar(elements_)
	{}

	StateTable( const StateTable& o)
		:m_ar(o.m_ar){}

	const Element& operator[]( int stateidx) const
	{
		return m_ar[ stateidx];
	}

private:
	const Element* m_ar;
};

}}//namespace
#endif

