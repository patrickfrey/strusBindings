/*
---------------------------------------------------------------------
    The C++ library strus implements basic operations to build
    a search engine for structured search on unstructured data.

    Copyright (C) 2015 Patrick Frey

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 3 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

--------------------------------------------------------------------

	The latest version of strus can be found at 'http://github.com/patrickfrey/strus'
	For documentation see 'http://patrickfrey.github.com/strus'

--------------------------------------------------------------------
*/
#include "private/wcharString.hpp"
#include "private/internationalization.hpp"
#include <cstdlib>
#include <cstdio>
#include "textwolf/xmlscanner.hpp"
#include "textwolf/cstringiterator.hpp"
#include "textwolf/charset.hpp"
#include <stdint.h>

#define MAXDUMPSIZE 2000

#ifdef _MSC_VER
#define IS_BIG_ENDIAN  0
#define IS_WCHAR_UCS4  0
#elif __GNUC__
#define IS_BIG_ENDIAN  (__ORDER_BIG_ENDIAN__)
#define IS_WCHAR_UCS4  (__SIZEOF_WCHAR_T__ == 4)
#else
#error Cannot determine endianness of platform
#endif

std::wstring convert_uft8string_to_wstring( const std::string& val)
{
	typedef textwolf::TextScanner<textwolf::CStringIterator,textwolf::charset::UTF8> ScannerUTF8;
	ScannerUTF8 itr(val);

	char cbuf[ 32];
#if __SIZEOF_WCHAR_T__ == 4
#if __ORDER_BIG_ENDIAN__
	textwolf::charset::UCS4BE wcout;
#else
	textwolf::charset::UCS4LE wcout;
#endif
#else
#if __ORDER_BIG_ENDIAN__
	textwolf::charset::UTF16BE wcout;
#else
	textwolf::charset::UTF16LE wcout;
#endif
#endif
	std::wstring rt;
	rt.reserve( val.size());
	textwolf::StaticBuffer sb( cbuf, sizeof(cbuf));

	textwolf::UChar chr;
	for (; 0!=(chr=*itr); ++itr)
	{
		if (chr == 0xFFFF)
		{
			throw strus::runtime_error( _TXT("multibyte to wide character conversion error"));
		}
		wcout.print( chr, sb);
		wchar_t const* wp = (wchar_t*)(void*)cbuf;
		for (std::size_t ii=0; ii < sb.size(); ii += sizeof(wchar_t), ++wp)
		{
			rt.push_back( *wp);
		}
		sb.clear();
	}
	return rt;
}

std::string convert_wstring_to_uft8string( const std::wstring& val)
{
	std::string rt;
	rt.reserve( val.size());
	textwolf::charset::UTF8 out;

	std::wstring::const_iterator vi = val.begin(), ve = val.end();
	for (; vi < ve; ++vi)
	{
		out.print( *vi, rt);
	}
	return rt;
}

