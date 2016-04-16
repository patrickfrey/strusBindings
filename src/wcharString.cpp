/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "private/wcharString.hpp"
#include "private/internationalization.hpp"
#include <cstdlib>
#include <cstdio>
#include "textwolf/xmlscanner.hpp"
#include "textwolf/cstringiterator.hpp"
#include "textwolf/charset.hpp"

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

