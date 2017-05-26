/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _PAPUGA_SERIALIZATION_HPP_INCLUDED
#define _PAPUGA_SERIALIZATION_HPP_INCLUDED
/// \brief Some functions on serialization using C++ features like STL
/// \file serialization.hpp
#include "papuga/typedefs.h"
#include "papuga/serialization.h"
#include "papuga/valueVariant.hpp"
#include <string>

namespace papuga {

/// \brief C++ representation of a papuga object view serialization
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

	/// \brief Iterator on a papuga object view serialization
	class const_iterator
	{
	public:
		/// \brief Constructor
		const_iterator( const papuga_Node* m_ar)
			:m_ptr(m_ar){}
		/// \brief Copy constructor
		const_iterator( const const_iterator& o)
			:m_ptr(o.m_ptr){}

		/// \brief Node access
		const papuga_Node& operator*() const			{return *m_ptr;}
		/// \brief Node access
		const papuga_Node* operator->() const			{return m_ptr;}
		/// \brief Prefix increment
		const_iterator& operator++()				{++m_ptr; return *this;}
		/// \brief Postfix increment
		const_iterator operator++(int)				{const_iterator rt(m_ptr); ++m_ptr; return rt;}
		/// \brief Prefix decrement
		const_iterator& operator--()				{--m_ptr; return *this;}
		/// \brief Postfix decrement
		const_iterator operator--(int)				{const_iterator rt(m_ptr); --m_ptr; return rt;}
		/// \brief Test for equality
		bool operator == (const const_iterator& o)		{return m_ptr==o.m_ptr;}
		/// \brief Test for inequality
		bool operator != (const const_iterator& o)		{return m_ptr!=o.m_ptr;}

		/// \brief Add offset
		const_iterator operator+( std::size_t ofs)		{return const_iterator(m_ptr+ofs);}
		/// \brief Subtract offset
		const_iterator operator-( std::size_t ofs)		{return const_iterator(m_ptr-ofs);}

	private:
		/// \brief Node array pointer reference
		papuga_Node const* m_ptr;
	};

	/// \brief Start iterator
	const_iterator begin() const					{return const_iterator(m_st.ar);}
	/// \brief End iterator
	const_iterator end() const					{return const_iterator(m_st.ar + m_st.arsize);}

	/// \brief Start iterator
	static const_iterator begin( const papuga_Serialization* cser)	{return const_iterator(cser->ar);}
	/// \brief End iterator
	static const_iterator end( const papuga_Serialization* cser)	{return const_iterator(cser->ar + cser->arsize);}

	/// \brief Pointer to internal structure
	const papuga_Serialization* cstruct() const			{return &m_st;}
	/// \brief Pointer to internal structure
	papuga_Serialization* cstruct()					{return &m_st;}

	/// \brief Print serialization to a string buffer in a readable form
	/// \param[in] serialization to print
	/// \param[out] error code in case of error
	/// \return the string with the serialization printed
	static std::string tostring( const papuga_Serialization& serialization, papuga_ErrorCode& errcode);

private:
	papuga_Serialization m_st;		///< internal representation of the serialization
};

}//namespace
#endif


