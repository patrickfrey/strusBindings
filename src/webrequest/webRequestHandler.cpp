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
#define DEFINE_SCHEMA( EXTNAME, SCHEMANAME)\
	static const Schema ## SCHEMANAME  schema ## SCHEMANAME;\
	if (!papuga_RequestHandler_add_schema( m_impl, EXTNAME, schema ## SCHEMANAME .impl())) throw std::bad_alloc();\
	if (!papuga_RequestHandler_allow_schema_access_all( m_impl, #EXTNAME, &errcode)) throw std::bad_alloc();

	DEFINE_SCHEMA( "init", CreateContext);
	DEFINE_SCHEMA( "create_storage", CreateStorage);
	DEFINE_SCHEMA( "destroy_storage", DestroyStorage);
	DEFINE_SCHEMA( "open_storage", OpenStorage);
}

WebRequestHandler::~WebRequestHandler()
{
	papuga_destroy_RequestHandler( m_impl);
}

static void setStatus( WebRequestAnswer& status, ErrorOperation operation, papuga_ErrorCode errcode)
{
	ErrorCause errcause = papugaErrorToErrorCause( errcode);
	const char* errstr = papuga_ErrorCode_tostring( errcode);
	int httpstatus = errorCauseToHttpStatus( errcause);
	status.setError( httpstatus, *ErrorCode( StrusComponentWebService, operation, errcause), errstr);
}

WebRequestContextInterface* WebRequestHandler::createRequestContext(
		const char* context,
		const char* schema,
		const char* role,
		WebRequestAnswer& status) const
{
	papuga_ErrorCode errcode = papuga_Ok;
	try
	{
		return new WebRequestContext( m_impl, context, schema, role);
	}
	catch (const std::bad_alloc&)
	{
		errcode = papuga_NoMemError;
	}
	catch (const WebRequestContext::Exception& err)
	{
		errcode = err.errcode();
	}
	catch (...)
	{
		errcode = papuga_UncaughtException;
	}
	setStatus( status, ErrorOperationBuildData, errcode);
	return NULL;
}


