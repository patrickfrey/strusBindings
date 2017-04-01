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
	struct Node
	{
		Tag tag;
		union
		{
			int index;
			const char* name;
		} value;

		Node( const Tag& tag_, const char* name_)
			:tag(tag_)
		{
			value.name = name_;
		}
		Node( const Tag& tag_, int index_)
			:tag(tag_)
		{
			value.index = index_;
		}
		Node( const Tag& tag_)
			:tag(tag_)
		{
			value.index = 0;
		}
		Node( const Node& o)
			:tag(o.tag)
		{
			value.name = o.value.name;
		}
	};

	Serialization()
		:m_ar(),m_valuear(){}
	Serialization( const Serialization& o)
		:m_ar(o.m_ar),m_valuear(o.m_valuear){}

	void clear()
	{
		m_ar.clear();
		m_valuear.clear();
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
		m_ar.push_back( Node( Value, m_valuear.size()));
		m_valuear.push_back( val);
	}

	class const_iterator
	{
	public:
		const_iterator( const std::vector<Node>& ar, const std::vector<ValueVariant>& var_, bool start)
			:m_ni(start?ar.begin():ar.end()),m_var(var_.data()){}
		const_iterator( const const_iterator& o)
			:m_ni(o.m_ni),m_var(o.m_var){}

		bool operator==( const const_iterator& o) const		{return m_ni == o.m_ni;}
		bool operator!=( const const_iterator& o) const		{return m_ni != o.m_ni;}

		const_iterator& operator++()				{++m_ni; return *this;}
		const_iterator operator++(int)				{const_iterator rt(*this); ++m_ni; return rt;}
		const_iterator& operator--()				{--m_ni; return *this;}
		const_iterator operator--(int)				{const_iterator rt(*this); --m_ni; return rt;}

	private:
		std::vector<Node>::const_iterator m_ni;
		ValueVariant const* m_var;
	};

private:
	std::vector<Node> m_ar;
	std::vector<ValueVariant> m_valuear;
};

}}//namespace
#endif


