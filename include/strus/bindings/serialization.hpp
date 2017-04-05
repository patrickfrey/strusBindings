/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDINGS_SERIALIZATION_HPP_INCLUDED
#define _STRUS_BINDINGS_SERIALIZATION_HPP_INCLUDED
/// \brief Serialization of structures for the language bindings
/// \file serialization.hpp
#include "strus/bindings/valueVariant.hpp"
#include <string>
#include <vector>

namespace strus {
namespace bindings {

class Serialization
{
public:
	enum Tag
	{
		Open,		///< Open a scope
		Index,		///< Open an array element
		Value,		///< Atomic value
		Close		///< Closes a scope
	};
	struct Node :public ValueVariant
	{
		Tag tag;

		Node( const Tag& tag_, const ValueVariant& value_)
			:ValueVariant(value_),tag(tag_)
		{}
		Node( const Tag& tag_, const char* value_)
			:ValueVariant(value_),tag(tag_)
		{}
		Node( const Tag& tag_, const int value_)
			:ValueVariant((ValueVariant::IntType)value_),tag(tag_)
		{}
		Node( const Tag& tag_)
			:ValueVariant(),tag(tag_)
		{}
		Node( const Node& o)
			:ValueVariant(o),tag(o.tag)
		{}
	};

	Serialization()
		:m_ar(){}
	Serialization( const Serialization& o)
		:m_ar(o.m_ar){}

	void clear()
	{
		m_ar.clear();
	}
	void pushOpen( const char* tagname)
	{
		m_ar.push_back( Node( Open, tagname));
	}
	void pushClose()
	{
		m_ar.push_back( Node( Close));
	}
	void pushIndex()
	{
		m_ar.push_back( Node( Index));
	}
	void pushValue( const ValueVariant& val)
	{
		m_ar.push_back( Node( Value, val));
	}

	typedef std::vector<Node>::const_iterator const_iterator;

	const_iterator begin() const			{return m_ar.begin();}
	const_iterator end() const			{return m_ar.end();}

	std::size_t size() const			{return m_ar.size();}
	const Node& operator[]( std::size_t i) const	{return m_ar[i];}

private:
	std::vector<Node> m_ar;
};

}}//namespace
#endif


