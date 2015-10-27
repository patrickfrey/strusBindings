/*
---------------------------------------------------------------------
    The template library textwolf implements an input iterator on
    a set of XML path expressions without backward references on an
    STL conforming input iterator as source. It does no buffering
    or read ahead and is dedicated for stream processing of XML
    for a small set of XML queries.
    Stream processing in this Object refers to processing the
    document without buffering anything but the current result token
    processed with its tag hierarchy information.

    Copyright (C) 2010,2011,2012,2013,2014 Patrick Frey

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3.0 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

--------------------------------------------------------------------

	The latest version of textwolf can be found at 'http://github.com/patrickfrey/textwolf'
	For documentation see 'http://patrickfrey.github.com/textwolf'

--------------------------------------------------------------------
*/
/// \file textwolf/wstringiterator.hpp
/// \brief textwolf iterator on wchar_t strings

#ifndef __TEXTWOLF_WSTRING_ITERATOR_HPP__
#define __TEXTWOLF_WSTRING_ITERATOR_HPP__
#include "textwolf/char.hpp"
#include "textwolf/byteorderconverter.hpp"
#include <string>
#include <cstring>
#include <cstdlib>
#include <arpa/inet.h>

/// \namespace textwolf
/// \brief Toplevel namespace of the library
namespace textwolf {

/// \class WStringIterator
/// \brief Input iterator on a constant wide character string returning null characters after EOF as required by textwolf scanners
class WStringIterator
{
public:
	/// \brief Default constructor
	WStringIterator()
		:m_null(0)
		,m_bufpos(0)
		,m_src(0)
		,m_size(0)
		,m_pos(0)
	{
		m_buf.val32 = 0;
	}

	/// \brief Constructor
	/// \param [in] src null terminated C string to iterate on
	/// \param [in] size number of bytes in the string to iterate on
	WStringIterator( const wchar_t* src, unsigned int size)
		:m_null(0)
		,m_bufpos(0)
		,m_src(src)
		,m_size(size)
		,m_pos(1)
	{
		m_buf.val32 = 0;
		ByteOrderConverter<sizeof(wchar_t)>::write( m_buf.ar, m_src[0]);
	}

	/// \brief Constructor
	/// \param [in] src string to iterate on
	WStringIterator( const wchar_t* src)
		:m_null(0)
		,m_bufpos(0)
		,m_src(src)
		,m_size(0)
		,m_pos(1)
	{
		for (;m_src[m_size]; ++m_size){} // m_size = wcslen( src) ~ C99
		m_buf.val32 = 0;
		ByteOrderConverter<sizeof(wchar_t)>::write( m_buf.ar, m_src[0]);
	}

	/// \brief Constructor
	/// \param [in] src string to iterate on
	WStringIterator( const std::wstring& src)
		:m_null(0)
		,m_bufpos(0)
		,m_src(src.c_str())
		,m_size(src.size())
		,m_pos(1)
	{
		m_buf.val32 = 0;
		ByteOrderConverter<sizeof(wchar_t)>::write( m_buf.ar, m_src[0]);
	}

	/// \brief Copy constructor
	/// \param [in] o iterator to copy
	WStringIterator( const WStringIterator& o)
		:m_null(o.m_null)
		,m_bufpos(o.m_bufpos)
		,m_src(o.m_src)
		,m_size(o.m_size)
		,m_pos(o.m_pos)
	{
		m_buf.val32 = o.m_buf.val32;
	}

	/// \brief Element access
	/// \return current character
	inline char operator* ()
	{
		return m_buf.ar[ m_bufpos];
	}

	/// \brief Preincrement
	inline WStringIterator& operator++()
	{
		if (++m_bufpos >= sizeof(wchar_t))
		{
			if (m_pos < m_size)
			{
				ByteOrderConverter<sizeof(wchar_t)>::write( m_buf.ar, m_src[ m_pos++]);
				m_bufpos = 0;
			}
			else
			{
				m_bufpos = 4;
			}
		}
		return *this;
	}

	inline int operator - (const WStringIterator& o) const
	{
		if (m_src != o.m_src) return 0;
		return (int)(m_pos - o.m_pos) * sizeof(wchar_t) + (int)m_bufpos - (int)o.m_bufpos;
	}

private:
	union
	{
		unsigned char ar[4];
		short val16;
		UChar val32;
	} m_buf;
	unsigned short m_null;
	unsigned short m_bufpos;
	const wchar_t* m_src;
	unsigned int m_size;
	unsigned int m_pos;
};

}//namespace
#endif
