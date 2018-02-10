/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Implementation of the interface for executing XML/JSON requests on the strus bindings
/// \file "webRequestHandler.cpp"
#include "webRequestHandler.hpp"
#include "webRequestContext.hpp"
#include "webRequestUtils.hpp"
#include "schemas.hpp"
#include "strus/webRequestLoggerInterface.hpp"
#include "strus/errorCodes.hpp"
#include "strus/base/local_ptr.hpp"
#include "strus/base/string_format.hpp"
#include "papuga/request.h"
#include "papuga/errors.h"
#include "papuga/typedefs.h"
#include "papuga/valueVariant.hpp"
#include "papuga/valueVariant.h"
#include "private/internationalization.hpp"
#include <vector>
#include <sstream>
#include <iostream>
#include <cstring>
#include <cstdarg>

using namespace strus;

#undef STRUS_LOWLEVEL_DEBUG

static std::vector<std::string> getLogArgument( std::size_t nof_arguments, va_list arguments, std::size_t nof_itypes, const papuga_RequestLogItem* itype)
{
	papuga_ErrorCode errcode = papuga_Ok;
	std::vector<std::string> rt( nof_itypes);
	std::size_t nofargs = 0;
	int ai=0, ae=nof_arguments;
	for(; ai < ae; ++ai)
	{
		typedef int inttype;
		typedef const char* charp;

		papuga_RequestLogItem aitype = (papuga_RequestLogItem)va_arg( arguments, inttype);
		int ei=0, ee=nof_itypes;
		for(; ei < ee && itype[ ei] != aitype; ++ei){}
		switch (aitype)
		{
			case papuga_LogItemClassName:
			case papuga_LogItemMethodName:
			case papuga_LogItemMessage:
				if (ei < ee) rt[ei] = va_arg( arguments,charp);
				break;
			case papuga_LogItemResult:
				if (ei < ee) rt[ei] = papuga::ValueVariant_tostring( *va_arg( arguments, papuga_ValueVariant*), errcode);
				break;
			case papuga_LogItemArgc:
			{
				nofargs = va_arg( arguments,size_t);
				if (ei < ee)
				{
					std::ostringstream num;
					num << nofargs;
					rt[ei] = num.str();
				}
				break;
			}
			case papuga_LogItemArgv:
			{
				if (ei < ee)
				{
					std::ostringstream argstr;
					papuga_ValueVariant* ar = va_arg( arguments, papuga_ValueVariant*);
					std::size_t ii=0, ie=nofargs;
					for (; ii!=ie; ++ii)
					{
						if (ii) argstr << " ";
						if (papuga_ValueVariant_isatomic( ar+ii))
						{
							argstr << '\"' << papuga::ValueVariant_tostring( ar[ii], errcode) << '\"';
						}
						else if (ar[ii].valuetype == papuga_TypeSerialization)
						{
							argstr << "{}";
						}
						else
						{
							argstr << "*";
						}
					}
					rt[ei] = argstr.str();
				}
				break;
			}
		}
	}
	return rt;
}

static void logMethodCall( void* self_, int nofItems, ...)
{
	WebRequestLoggerInterface* self = (WebRequestLoggerInterface*)self_;
	va_list arguments;
	va_start( arguments, nofItems );

	enum {nof_itypes=5};
	static const papuga_RequestLogItem itypes[nof_itypes] = {
		papuga_LogItemClassName,
		papuga_LogItemMethodName,
		papuga_LogItemArgv,
		papuga_LogItemResult
	};
	try
	{
		std::vector<std::string> args = getLogArgument( nofItems, arguments, nof_itypes, itypes);
		self->logMethodCall( args[0], args[1], args[2], args[3]);
	}
	catch (...){}
	va_end( arguments);
}

WebRequestHandler::WebRequestHandler( WebRequestLoggerInterface* logger_)
{
	papuga_ErrorCode errcode = papuga_Ok;
	std::memset( &m_logger, 0, sizeof(m_logger));
	m_logger.self = logger_;
	int mask = logger_->logMask();
	if (!!(mask & (int)WebRequestLoggerInterface::LogMethodCalls)) m_logger.logMethodCall = &logMethodCall;
	m_impl = papuga_create_RequestHandler( &m_logger);
	if (!m_impl) throw std::bad_alloc();

	using namespace strus::webrequest;
#define DEFINE_SCHEMA( EXTNAME, SCHEMANAME, ALLOW)\
	static const Schema ## SCHEMANAME  schema ## SCHEMANAME;\
	if (!papuga_RequestHandler_add_schema( m_impl, EXTNAME, schema ## SCHEMANAME .impl())) throw std::bad_alloc();\
	if (!papuga_RequestHandler_allow_schema_access( m_impl, EXTNAME, ALLOW, &errcode)) throw std::bad_alloc();

	DEFINE_SCHEMA( "init", CreateContext, "config");
	DEFINE_SCHEMA( "create_storage", CreateStorage, "config");
	DEFINE_SCHEMA( "destroy_storage", DestroyStorage, "config");
	DEFINE_SCHEMA( "init_storage", OpenStorage, "config");
}

WebRequestHandler::~WebRequestHandler()
{
	papuga_destroy_RequestHandler( m_impl);
}

bool WebRequestHandler::hasSchema( const char* schema) const
{
	return papuga_RequestHandler_has_schema( m_impl, schema);
}

static void setStatus( WebRequestAnswer& status, ErrorOperation operation, papuga_ErrorCode errcode, const char* errmsg=0)
{
	ErrorCause errcause = papugaErrorToErrorCause( errcode);
	const char* errstr = papuga_ErrorCode_tostring( errcode);
	int httpstatus = errorCauseToHttpStatus( errcause);
	if (errmsg)
	{
		char errbuf[ 1024];
		if (sizeof(errbuf) >= std::snprintf( errbuf, sizeof(errbuf), "%s, %s", errmsg, errstr))
		{
			errbuf[ sizeof(errbuf)-1] = 0;
		}
		status.setError( httpstatus, *ErrorCode( StrusComponentWebService, operation, errcause), errbuf, true);
	}
	else
	{
		status.setError( httpstatus, *ErrorCode( StrusComponentWebService, operation, errcause), errstr);
	}
}

WebRequestContext* WebRequestHandler::createContext_( const char* context, const char* schema, const char* role, const char* accepted_charset, const char* accepted_doctype, WebRequestAnswer& status) const
{
	try
	{
		return new WebRequestContext( m_impl, context, schema, role, accepted_charset, accepted_doctype);
	}
	catch (const std::bad_alloc&)
	{
		setStatus( status, ErrorOperationBuildData, papuga_NoMemError);
	}
	catch (const WebRequestContext::Exception& err)
	{
		setStatus( status, ErrorOperationBuildData, err.errcode(), err.errmsg());
	}
	catch (...)
	{
		setStatus( status, ErrorOperationBuildData, papuga_UncaughtException);
	}
	return NULL;
}

WebRequestContextInterface* WebRequestHandler::createContext(
		const char* context,
		const char* schema,
		const char* role,
		const char* accepted_charset,
		const char* accepted_doctype,
		WebRequestAnswer& status) const
{
	return createContext_( context, schema, role, accepted_charset, accepted_doctype, status);
}

bool WebRequestHandler::loadConfiguration(
			const char* destContextName,
			const char* destContextSchemaPrefix,
			const char* srcContextName,
			const char* schema,
			const WebRequestContent& content,
			WebRequestAnswer& status)
{
#ifdef STRUS_LOWLEVEL_DEBUG
	std::string co( webRequestContent_tostring( content));
	if (co.size() > 200) co.resize( 200);
	std::cerr << strus::string_format( "load configuration: context %s %s <- %s, schema %s, doctype %s, encoding %s, content '%s'",
						destContextSchemaPrefix, destContextName, srcContextName, schema,
						content.doctype(), content.charset(), co.c_str()) << std::endl;
#endif
	strus::local_ptr<WebRequestContext> ctx( createContext_( srcContextName, schema, "config"/*role*/, "UTF-8"/*accepted_charset*/, "application/json"/*accepted_doctype*/, status));
	if (!ctx.get()) return false;

	strus::unique_lock lock( m_mutex);
	if (ctx->executeConfig( destContextName, destContextSchemaPrefix, content, status))
	{
		papuga_ErrorCode errcode = papuga_Ok;
#ifdef STRUS_LOWLEVEL_DEBUG
		std::cerr << strus::string_format( "allow acccess to '%s'", srcContextName ? "*":"config") << std::endl;
#endif
		if (!papuga_RequestHandler_allow_context_access( m_impl, destContextName, srcContextName ? "*":"config", &errcode))
		{
			setStatus( status, ErrorOperationConfiguration, errcode);
			return false;
		}
		return true;
	}
	else
	{
		return false;
	}
}




