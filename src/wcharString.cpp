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
#include "private/internationalization.hpp"
#include <cstdlib>

std::wstring convert_UTF8_to_UTF16( const std::string& val)
{
	std::wstring rt;
	rt.reserve( val.size());
	std::size_t ii=0,nn=val.size();
	int clen;
	wchar_t wc;
	while (0<(clen=std::mbtowc( &wc, val.c_str() + ii, nn - ii)))
	{
		rt.push_back( wc);
		ii += clen;
	}
	if (clen < 0)
	{
		throw strus::runtime_error( "multibyte to wide character conversion error");
	}
	return rt;
}

std::string convert_UTF16_to_UTF8( const std::wstring& val)
{
	std::string rt;
	rt.reserve( val.size());
	char cbuf[8];
	std::wstring::const_iterator vi = val.begin(), ve = val.end();
	int clen = 0;
	for (; vi < ve && 0<(clen=std::wctomb( cbuf, *vi)); ++vi)
	{
		rt.append( cbuf, clen);
	}
	if (clen < 0)
	{
		throw strus::runtime_error( "wide to multibyte character conversion error");
	}
	return rt;
}

