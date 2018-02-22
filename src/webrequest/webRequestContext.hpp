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
#include "papuga/requestHandler.h"
#include "papuga/requestParser.h"
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
		const char* accepted_charset,
		const char* accepted_doctype);
	virtual ~WebRequestContext();

	virtual bool executeContent(
			const std::string& context,
			const std::string& schema,
			const WebRequestContent& content,
			WebRequestAnswer& answer);

	virtual bool debugContent(
			const std::string& context,
			const std::string& schema,
			const WebRequestContent& content,
			WebRequestAnswer& answer);

	virtual bool executeList(
			const std::vector<std::string>& path,
			WebRequestAnswer& answer);

	virtual bool executeView(
			const std::vector<std::string>& path,
			WebRequestAnswer& answer);

public:/*WebRequestHandler*/
	papuga_RequestContext* impl()			{return &m_context;}

private:
	bool initContentRequest( const char* context, const char* schema);
	bool feedContentRequest( WebRequestAnswer& answer, const WebRequestContent& content);
	bool debugContentRequest( WebRequestAnswer& answer);
	bool executeContentRequest( WebRequestAnswer& answer, const WebRequestContent& content);
	bool setResultContentType( WebRequestAnswer& answer, papuga_StringEncoding default_encoding, WebRequestContent::Type default_doctype);
	bool getContentRequestResult( WebRequestAnswer& answer);

	bool callListMethod( papuga_ValueVariant* obj, WebRequestAnswer& answer) const;
	bool callViewMethod( papuga_ValueVariant* obj, WebRequestAnswer& answer) const;

private:
	const WebRequestHandler* m_handler;
	papuga_Allocator m_allocator;
	papuga_RequestContext m_context;
	papuga_Request* m_request;
	papuga_StringEncoding m_encoding;
	papuga_ContentType m_doctype;
	const char* m_doctypestr;
	const papuga_RequestAutomaton* m_atm;
	papuga_StringEncoding m_result_encoding;
	WebRequestContent::Type m_result_doctype;
	papuga_ErrorCode m_errcode;
	papuga_ErrorBuffer m_errbuf;
	const char* m_accepted_charset;
	const char* m_accepted_doctype;
	char m_errbuf_mem[ 4096];
	char m_allocator_mem[ 1<<14];
};

}//namespace
#endif


