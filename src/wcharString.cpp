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

#ifdef _MSC_VER
#define IS_BIG_ENDIAN  0
#elif __GNUC__
#define IS_BIG_ENDIAN  (__ORDER_BIG_ENDIAN__)
#else
#error Cannot determine endianness of platform
#endif

#if IS_BIG_ENDIAN
#define W16CHARSET textwolf::charset::UTF16BE
#else
#define W16CHARSET textwolf::charset::UTF16LE
#endif

using namespace strus;

std::basic_string<uint16_t> strus::convert_uft8string_to_w16string( const char* str, std::size_t strsize)
{
	typedef textwolf::TextScanner<textwolf::CStringIterator,textwolf::charset::UTF8> ScannerUTF8;
	ScannerUTF8 itr( textwolf::CStringIterator( str, strsize));

	char cbuf[ 32];

	W16CHARSET wcout;
	std::basic_string<uint16_t> rt;
	rt.reserve( strsize + (strsize >> 4));
	textwolf::StaticBuffer sb( cbuf, sizeof(cbuf));

	textwolf::UChar chr;
	for (; 0!=(chr=*itr); ++itr)
	{
		if (chr == 0xFFFF) continue;

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

std::basic_string<uint16_t> strus::convert_uft8string_to_w16string( const std::string& val)
{
	return convert_uft8string_to_w16string( val.c_str(), val.size());
}

std::string strus::convert_w16string_to_uft8string( const uint16_t* str, std::size_t strsize)
{
	typedef textwolf::TextScanner<textwolf::CStringIterator,W16CHARSET> ScannerW16String;
	ScannerW16String itr( textwolf::CStringIterator( reinterpret_cast<const char*>(str), strsize*2));

	std::string rt;
	rt.reserve( strsize + (strsize >> 4));

	textwolf::charset::UTF8 u8out;

	textwolf::UChar chr;
	for (; 0!=(chr=*itr); ++itr)
	{
		u8out.print( chr, rt);
	}
	return rt;
}

std::string strus::convert_w16string_to_uft8string( const std::basic_string<uint16_t>& val)
{
	return convert_w16string_to_uft8string( val.c_str(), val.size());
}

