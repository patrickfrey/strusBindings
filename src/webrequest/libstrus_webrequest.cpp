/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Library for handling XML/JSON requests
/// \file libstrus_webrequest.cpp
#include "strus/lib/webrequest.hpp"
#include "strus/base/dll_tags.hpp"
#include "webRequestHandler.hpp"
#include "webRequestUtils.hpp"
#include "papuga/valueVariant.h"
#include "papuga/encoding.h"
#include <new>

/// \brief strus toplevel namespace
using namespace strus;

DLL_PUBLIC WebRequestHandlerInterface* strus::createWebRequestHandler( WebRequestLoggerInterface* logger, const std::string& html_head)
{
	return new (std::nothrow) WebRequestHandler( logger, html_head);
}

DLL_PUBLIC const char* strus::convertContentCharset( const char* charsetname, char* destbuf, std::size_t destbufsize,  std::size_t& length, const char* src, std::size_t srcsize)
{
	papuga_StringEncoding encoding;
	if (!papuga_getStringEncodingFromName( &encoding, charsetname)) return NULL;

	papuga_ValueVariant res;
	papuga_init_ValueVariant_string( &res, src, srcsize);

	std::size_t usize = papuga_StringEncoding_unit_size( encoding);
	if (!usize) return NULL;

	std::size_t pos = (std::size_t)destbuf % usize;
	for (; pos % usize != 0; ++pos){}
	if (pos >= destbufsize) return NULL;
	std::size_t outlen;
	papuga_ErrorCode errcode = papuga_Ok;
	const char* rt = (const char*)papuga_ValueVariant_tostring_enc( &res, encoding, destbuf+pos, destbufsize-pos, &outlen, &errcode);
	length = outlen * usize;
	return rt;
}

DLL_PUBLIC const char* strus::selectAcceptedCharset( const char* http_accept_charset)
{
	papuga_StringEncoding enc = strus::getResultStringEncoding( http_accept_charset, papuga_UTF8);
	if (enc == papuga_Binary) return NULL;
	return papuga_stringEncodingName( enc);
}

DLL_PUBLIC WebRequestContent::Type strus::selectAcceptedContentType( const char* http_accept)
{
	return strus::getResultContentType( http_accept, WebRequestContent::HTML);
}


