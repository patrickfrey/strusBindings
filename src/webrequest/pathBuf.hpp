/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Buffer with iterator for a path of a request
/// \file "pathBuf.hpp"
#ifndef _STRUS_WEB_REQUEST_PATH_BUF_HPP_INCLUDED
#define _STRUS_WEB_REQUEST_PATH_BUF_HPP_INCLUDED
#include <cstddef>
#include <cstring>

namespace strus {

class PathBuf
{
public:
	PathBuf()
	{
		buf[0] = 0;
		itr = buf;
	}
	explicit PathBuf( char const* pt)
	{
		while (*pt == '/') ++pt;
		if ((int)sizeof(buf)-1 <= std::snprintf( buf, sizeof(buf), "%s", pt)) throw std::bad_alloc();
		itr = buf;
	}

	bool startsWith( const char* prefix, int prefixlen) const noexcept
	{
		char* itrnext = std::strchr( itr, '/');
		if (!itrnext) itrnext = std::strchr( itr, '\0');
		return (prefixlen == itrnext-itr && prefix[0] == itr[0] && 0==std::memcmp( prefix, itr, prefixlen));
	}

	bool hasMore() const noexcept
	{
		return itr[0];
	}

	const char* getNext()
	{
		if (!itr[0]) return 0;
		char const* rt = itr;
		char* itrnext = std::strchr( itr, '/');
		if (itrnext)
		{
			*itrnext = 0;
			itr = itrnext+1;
			while (*itr == '/') ++itr;
		}
		else
		{
			itr = std::strchr( itr, '\0');
		}
		return rt;
	}

	const char* getRest()
	{
		char const* rt = itr;
		itr = std::strchr( itr, '\0');
		char* enditr = itr;
		while (enditr != buf && *(enditr-1) == '/') --enditr;
		*enditr = '\0';
		return rt;
	}

	const char* rest()
	{
		char* enditr = std::strchr( itr, '\0');
		while (enditr != buf && *(enditr-1) == '/') --enditr;
		*enditr = '\0';
		return itr;
	}

private:
	char* itr;
	char buf[ 512];
};

} //namespace
#endif

