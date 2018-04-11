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
#include "strus/errorBufferInterface.hpp"
#include "webRequestHandler.hpp"
#include "webRequestUtils.hpp"
#include "private/internationalization.hpp"
#include "papuga/valueVariant.h"
#include "papuga/encoding.h"
#include <new>
#include <stdexcept>

/// \brief strus toplevel namespace
using namespace strus;

DLL_PUBLIC WebRequestHandlerInterface* strus::createWebRequestHandler(
		WebRequestLoggerInterface* logger,
		const std::string& html_head,
		const std::string& config_store_dir,
		const std::string& config,
		int maxIdleTime,
		int nofTransactionsPerSeconds,
		ErrorBufferInterface* errorhnd)
{
	try
	{
		return new WebRequestHandler( logger, html_head, config_store_dir, config, maxIdleTime, nofTransactionsPerSeconds);
	}
	catch (const std::bad_alloc&)
	{
		errorhnd->report( ErrorCodeOutOfMem, _TXT("out of memory"));
		return NULL;
	}
	catch (const std::runtime_error& err)
	{
		errorhnd->report( ErrorCodeRuntimeError, _TXT("error creating web request handler: %s"), err.what());
		return NULL;
	}
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

DLL_PUBLIC const char* strus::guessContentType( const char* content, std::size_t contentsize)
{
	papuga_ContentType ct = papuga_guess_ContentType( content, contentsize);
	return (ct == papuga_ContentType_Unknown) ? NULL : papuga_ContentType_mime(ct);
}

DLL_PUBLIC const char* strus::guessCharsetEncoding( const char* content, std::size_t contentsize)
{
	papuga_StringEncoding enc = papuga_guess_StringEncoding( content, contentsize);
	return (enc == papuga_Binary) ? NULL : papuga_stringEncodingName( enc);
}


