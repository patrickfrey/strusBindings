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
#include "strus/base/local_ptr.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/webRequestLoggerInterface.hpp"
#include "strus/webRequestHandlerInterface.hpp"
#include "webRequestHandler.hpp"
#include "webRequestUtils.hpp"
#include "curlEventLoop.hpp"
#include "private/internationalization.hpp"
#include "papuga/valueVariant.h"
#include "papuga/encoding.h"
#include <new>
#include <stdexcept>

/// \brief strus toplevel namespace
using namespace strus;

class WebRequestLogger_null
	:public WebRequestLoggerInterface
{
public:
	virtual ~WebRequestLogger_null(){}

	virtual int logMask() const {return 0;}
	virtual int structDepth() const {return 0;}
	virtual void logRequest( const char* content, std::size_t contentsize) {}
	virtual void logRequestType( const char* title, const char* procdescr, const char* contextType, const char* contextName){}
	virtual void logRequestAnswer( const char* content, std::size_t contentsize){}
	virtual void logPutConfiguration( const char* type, const char* name, const std::string& configstr) {}
	virtual void logDelegateRequest( const char* address, const char* method, const char* content, std::size_t contentsize) {}
	virtual void logAction( const char* type, const char* name, const char* action) {}
	virtual void logContentEvent( const char* title, const char* item, const char* content, std::size_t contentsize) {}
	virtual void logConnectionEvent( const char* content){}
	virtual void logConnectionState( const char* state, int arg){}

	virtual void logWarning( const char* warnmsg){}
	virtual void logError( const char* errmsg){}
	virtual void logContextInfoMessages( const char* content){}
};

class WebRequestEventLoop_null
	:public WebRequestEventLoopInterface
{
public:
	virtual ~WebRequestEventLoop_null(){}

	virtual bool start(){return false;}
	virtual void stop(){}
	virtual bool send(
			const char* address,
			const char* method,
			const char* contentstr,
			size_t contentlen,
			WebRequestDelegateContextInterface* receiver) {return false;}
	virtual bool addTickerEvent( void* obj, TickerFunction func)  {return true;}
	virtual void handleException( const char* msg) {}
	virtual long time() const {return 0;}
};

static WebRequestLogger_null g_logger_null;
static WebRequestEventLoop_null g_eventlopp_null;


DLL_PUBLIC WebRequestEventLoopInterface* strus::createCurlEventLoop( WebRequestLoggerInterface* logger, int timeout, int maxDelegateTotalConn, int maxDelegateHostConn, ErrorBufferInterface* errorhnd)
{
	try
	{
		return new CurlEventLoop( logger, timeout, maxDelegateTotalConn, maxDelegateHostConn);
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

DLL_PUBLIC WebRequestHandlerInterface* strus::createWebRequestHandler(
		WebRequestEventLoopInterface* eventloop,
		WebRequestLoggerInterface* logger,
		const std::string& html_head,
		const std::string& config_store_dir,
		const std::string& script_dir,
		const std::string& schema_dir,
		const std::string& service_name,
		int port,
		bool beautifiedOutput,
		int maxIdleTime,
		int nofTransactionsPerSeconds,
		ErrorBufferInterface* errorhnd)
{
	try
	{
		if (!logger) logger = &g_logger_null;
		return new WebRequestHandler( eventloop, logger, html_head, config_store_dir, script_dir, schema_dir, service_name, port, beautifiedOutput, maxIdleTime, nofTransactionsPerSeconds);
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

DLL_PUBLIC const char* strus::convertContentCharset( const char* charsetname, char* destbuf, std::size_t destbufsize, std::size_t& length, const char* src, std::size_t srcsize)
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
	papuga_ErrorCode errcode = papuga_Ok;
	const char* rt = (const char*)papuga_ValueVariant_tostring_enc( &res, encoding, destbuf+pos, destbufsize-pos, &length, &errcode);
	return rt;
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


