/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDINGS_PAPUGA_SERIALIZATION_HPP_INCLUDED
#define _STRUS_BINDINGS_PAPUGA_SERIALIZATION_HPP_INCLUDED
/// \brief C++ Interface to papuga serialization
/// \file serialization.hpp
#include "papuga/serialization.h"
#include <stdexcept>

namespace strus {
namespace bindings {

class Serialization
{
public:
	/// \brief Constructor
	Serialization()
	{
		papuga_init_Serialization( &m_st);
	}
	/// \brief Copy constructor
	Serialization( const Serialization& o)
	{
		papuga_init_Serialization_copy( &m_st, &o.m_st);
	}

	/// \brief Destructor
	~Serialization()
	{
		papuga_destroy_Serialization( &m_st);
	}

	class const_iterator
	{
	public:
		const_iterator( const papuga_Node* m_ar)
			:m_ptr(m_ar){}
		const_iterator( const const_iterator& o)
			:m_ptr(o.m_ptr){}

		const papuga_Node& operator*() const		{return *m_ptr;}
		const papuga_Node* operator->() const		{return m_ptr;}
		const_iterator& operator++()			{++m_ptr; return *this;}
		const_iterator operator++(int)			{const_iterator rt(m_ptr); ++m_ptr; return rt;}
		const_iterator& operator--()			{--m_ptr; return *this;}
		const_iterator operator--(int)			{const_iterator rt(m_ptr); --m_ptr; return rt;}
		bool operator == (const const_iterator& o)	{return m_ptr==o.m_ptr;}
		bool operator != (const const_iterator& o)	{return m_ptr!=o.m_ptr;}

		const_iterator operator+( std::size_t ofs)	{return const_iterator(m_ptr+ofs);}
		const_iterator operator-( std::size_t ofs)	{return const_iterator(m_ptr-ofs);}

	private:
		papuga_Node const* m_ptr;
	};

	const_iterator begin() const				{return const_iterator(m_st.ar);}
	const_iterator end() const				{return const_iterator(m_st.ar + m_st.arsize);}

	static const_iterator begin( papuga_Serialization* cser){return const_iterator(cser->ar);}
	static const_iterator end( papuga_Serialization* cser)	{return const_iterator(cser->ar + cser->arsize);}

	const papuga_Serialization* cstruct() const		{return &m_st;}
	papuga_Serialization* cstruct()				{return &m_st;}

private:
	papuga_Serialization m_st;
};

}} //namespace
#endif

