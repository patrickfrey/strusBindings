/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _PAPUGA_SERIALIZATION_HPP_INCLUDED
#define _PAPUGA_SERIALIZATION_HPP_INCLUDED
/// \brief Serialization of structures for papuga language bindings
/// \file serialization.hpp
#include "papuga/valueVariant.hpp"
#include <string>
#include <vector>

namespace papuga {

class Serialization
{
public:
	enum Tag
	{
		Open,		///< Open a scope
		Close,		///< Closes a scope
		Name,		///< The name of the following value (Value) or structure (Open)
		Value		///< Atomic value
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
	void pushOpen()
	{
		m_ar.push_back( Node( Open));
	}
	void pushClose()
	{
		m_ar.push_back( Node( Close));
	}
	void pushName( const char* tagname)
	{
		m_ar.push_back( Node( Name, tagname));
	}
	void pushName( const std::string& tagname)
	{
		m_ar.push_back( Node( Name, tagname));
	}
	void pushValue( const ValueVariant& val)
	{
		m_ar.push_back( Node( Value, val));
	}
	void append( const Serialization& val)
	{
		m_ar.insert( m_ar.end(), val.m_ar.begin(), val.m_ar.end());
	}

	typedef std::vector<Node>::const_iterator const_iterator;

	const_iterator begin() const			{return m_ar.begin();}
	const_iterator end() const			{return m_ar.end();}

	std::size_t size() const			{return m_ar.size();}
	const Node& operator[]( std::size_t i) const	{return m_ar[i];}

	static bool isLabeled( const_iterator si, const const_iterator& se)
	{
		for (; si != se; ++si)
		{
			if (si->tag == Name) return true;
			if (si->tag == Value) return false;
		}
		return false;
	}
	bool isLabeled() const
	{
		return isLabeled( m_ar.begin(), m_ar.end());
	}

private:
	std::vector<Node> m_ar;
};

}//namespace
#endif


