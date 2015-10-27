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
/// \file textwolf/byteorderconverter.hpp
/// \brief converter for byte order translation

#ifndef __TEXTWOLF_BYTEORDER_CONVERTER_ITERATOR_HPP__
#define __TEXTWOLF_BYTEORDER_CONVERTER_ITERATOR_HPP__
#include "textwolf/char.hpp"
#include <arpa/inet.h>

/// \namespace textwolf
/// \brief Toplevel namespace of the library
namespace textwolf {

/// \brief Abstract base template
template<unsigned char Size>
class ByteOrderConverter{};

/// \brief Converter for wchar_t of 4 bytes size
template<>
class ByteOrderConverter<2>
{
public:
	static void write( unsigned char* buf, wchar_t ch)
	{
		unsigned short val = htons( ch);
		buf[0] = val >> 8;
		buf[1] = val & 0xff;
	}
	static unsigned short read( unsigned char const* buf)
	{
		return htons( ((unsigned short)buf[0] << 8) + buf[1]);
	}
};

/// \brief Converter for wchar_t of 4 bytes size
template<>
class ByteOrderConverter<4>
{
public:
	static void write( unsigned char* buf, wchar_t ch)
	{
		UChar val = htonl( ch);
		buf[0] = val >> 24;
		buf[1] = (val >> 16) & 0xff;
		buf[2] = (val >> 8)  & 0xff;
		buf[3] = val & 0xff;
	}
	static UChar read( unsigned char const* buf)
	{
		return htons( ((UChar)buf[0] << 24) + ((UChar)buf[1] << 16) + ((UChar)buf[2] << 8) + buf[3]);
	}
};

}
#endif
