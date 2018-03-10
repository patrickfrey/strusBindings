/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Implementation of the interface for executing XML/JSON requests on the strus bindings
/// \file "webRequestContext.hpp"
#ifndef _STRUS_WEB_REQUEST_CONTEXT_IMPL_HPP_INCLUDED
#define _STRUS_WEB_REQUEST_CONTEXT_IMPL_HPP_INCLUDED
#include "strus/webRequestContextInterface.hpp"
#include "strus/webRequestLoggerInterface.hpp"
#include "papuga/requestHandler.h"
#include "papuga/requestParser.h"
#include "papuga/request.h"
#include "papuga/typedefs.h"
#include <stdexcept>

namespace strus
{

/// \brief Forward declaration
class WebRequestHandler;

/// \brief Implementation of the interface for executing an XML/JSON request on the strus bindings
class WebRequestContext
	:public WebRequestContextInterface
{
public:
	WebRequestContext(
		const WebRequestHandler* handler_,
		WebRequestLoggerInterface* logger_,
		const char* accepted_charset,
		const char* accepted_doctype);
	virtual ~WebRequestContext();

	virtual bool executeContent(
			const char* contextType,
			const char* contextName,
			const char* schema,
			const WebRequestContent& content,
			WebRequestAnswer& answer);

	virtual bool debugContent(
			const char* contextType,
			const char* contextName,
			const char* schema,
			const WebRequestContent& content,
			WebRequestAnswer& answer);

	virtual bool executeList(
			const char* path_,
			WebRequestAnswer& answer);

	virtual bool executeView(
			const char* path_,
			WebRequestAnswer& answer);

public:/*WebRequestHandler*/
	papuga_RequestContext* impl()			{return &m_context;}

private:
	bool initContentRequest( WebRequestAnswer& answer, const char* contextType, const char* contextName, const char* schema);
	bool feedContentRequest( WebRequestAnswer& answer, const WebRequestContent& content);
	bool debugContentRequest( WebRequestAnswer& answer);
	bool executeContentRequest( WebRequestAnswer& answer, const WebRequestContent& content);
	bool setResultContentType( WebRequestAnswer& answer, papuga_StringEncoding default_encoding, WebRequestContent::Type default_doctype);
	bool getContentRequestResult( WebRequestAnswer& answer);

	bool callListMethod( const papuga_ValueVariant* obj, const char* path, WebRequestAnswer& answer);
	bool callViewMethod( const papuga_ValueVariant* obj, const char* path, papuga_ValueVariant& result, WebRequestAnswer& answer);
	bool callViewMethod( const papuga_ValueVariant* obj, const char* path, WebRequestAnswer& answer);
	bool callMethod( void* self, const papuga_RequestMethodId& mid, const char* path_, papuga_ValueVariant& result, WebRequestAnswer& answer);

	bool dumpViewAll( papuga_Serialization* ser, WebRequestAnswer& answer);
	bool dumpViewType( const char* type_, papuga_Serialization* ser, WebRequestAnswer& answer);
	bool dumpViewName( const char* type_, const char* name_, papuga_Serialization* ser, WebRequestAnswer& answer);
	bool dumpViewVar( const papuga_RequestContext* context, const char* var_, papuga_Serialization* ser, WebRequestAnswer& answer);

private:
	const WebRequestHandler* m_handler;
	WebRequestLoggerInterface* m_logger;
	papuga_Allocator m_allocator;
	papuga_RequestContext m_context;
	papuga_Request* m_request;
	papuga_StringEncoding m_encoding;
	papuga_ContentType m_doctype;
	const char* m_doctypestr;
	const papuga_RequestAutomaton* m_atm;
	papuga_StringEncoding m_result_encoding;
	WebRequestContent::Type m_result_doctype;
	papuga_ErrorBuffer m_errbuf;
	const char* m_accepted_charset;
	const char* m_accepted_doctype;
	char m_errbuf_mem[ 4096];
	char m_allocator_mem[ 1<<14];
};

}//namespace
#endif


