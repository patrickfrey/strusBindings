/*
---------------------------------------------------------------------
    The C++ library strus implements basic operations to build
    a search engine for structured search on unstructured data.

    Copyright (C) 2013,2014 Patrick Frey

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

--------------------------------------------------------------------

	The latest version of strus can be found at 'http://github.com/patrickfrey/strus'
	For documentation see 'http://patrickfrey.github.com/strus'

--------------------------------------------------------------------
*/
#include "private/wcharString.hpp"
#include "textwolf/textscanner.hpp"
#include "textwolf/charset_utf8.hpp"
#include "textwolf/charset_utf16.hpp"
#include "textwolf/cstringiterator.hpp"
#include "textwolf/wstringiterator.hpp"
#include "textwolf/staticbuffer.hpp"

std::wstring convert_UTF8_to_UTF16( const std::string& val)
{
	std::wstring rt;
	typedef textwolf::CStringIterator SrcIterator;
	typedef textwolf::TextScanner<SrcIterator,textwolf::charset::UTF8> Scanner;
	SrcIterator src( val.c_str(), val.size());
	Scanner itr( src);
	char charbufmem[ 16];
	textwolf::StaticBuffer charbuf( charbufmem, sizeof(charbufmem));
	textwolf::charset::UTF16<> out;

	while (*itr)
	{
		textwolf::UChar ch = *itr;
		charbuf.clear();
		out.print( ch, charbuf);
		const char* ptr = charbuf.ptr();
		std::size_t ii=0, nn=charbuf.size();
		for (; ii<nn; ii+=sizeof(wchar_t))
		{
			wchar_t ch = textwolf::ByteOrderConverter<sizeof(wchar_t)>::read( (unsigned const char*)(ptr+ii));
			rt.push_back( ch);
		}
		++itr;
	}
	return rt;
}

std::string convert_UTF16_to_UTF8( const std::wstring& val)
{
	std::string rt;
	typedef textwolf::WStringIterator SrcIterator;
	typedef textwolf::TextScanner<SrcIterator,textwolf::charset::UTF16<> > Scanner;
	SrcIterator src( val.c_str(), val.size());
	Scanner itr( src);
	textwolf::charset::UTF8 out;

	while (*itr)
	{
		textwolf::UChar ch = *itr;
		out.print( ch, rt);
		++itr;
	}
	return rt;
}

