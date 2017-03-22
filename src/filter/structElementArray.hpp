/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_FILTER_STRUCTELEMENT_ARRAY_HPP_INCLUDED
#define _STRUS_BINDING_FILTER_STRUCTELEMENT_ARRAY_HPP_INCLUDED
/// \brief Array of structure element names for filters
/// \file structElementArray.hpp
#include "strus/binding/valueVariant.hpp"
#include <vector>

/// \brief strus toplevel namespace
namespace strus {
namespace filter {

/// \brief Static array of tag names as value variants
class StructElementArray
{
public:
	StructElementArray()
	{
		m_ar.push_back( binding::ValueVariant());
	}

	StructElementArray( const char** names)
	{
		char const** nitr = names;
		for (; *nitr; ++nitr) m_ar.push_back( *nitr);
		m_ar.push_back( binding::ValueVariant());
	}

	const binding::ValueVariant& operator[]( std::size_t idx) const
	{
		return m_ar[ idx];
	}

	unsigned int size() const
	{
		return m_ar.size();
	}

private:
	std::vector<binding::ValueVariant> m_ar;
};

}}//namespace
#endif

