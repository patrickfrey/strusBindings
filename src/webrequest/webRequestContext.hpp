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
#include "configurationHandler.hpp"
#include "transaction.hpp"
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
		WebRequestHandler* handler_,
		WebRequestLoggerInterface* logger_,
		ConfigurationHandler* confighandler_,
		TransactionPool* transactionPool_,
		const char* accepted_charset,
		const char* accepted_doctype,
		const char* html_base_href);

	virtual ~WebRequestContext();

	virtual bool executeRequest(
			const char* method,
			const char* path,
			const WebRequestContent& content,
			WebRequestAnswer& answer);

	virtual bool getMessageAnswer(
			const std::string& message,
			WebRequestAnswer& answer);

	virtual bool executeLoadMainConfiguration( const WebRequestContent& content, WebRequestAnswer& answer);
	virtual bool executeLoadSubConfiguration( const char* typenam, const char* contextnam, const WebRequestContent& content, WebRequestAnswer& answer);

private:
	bool executeOPTIONS( const char* path, const WebRequestContent& content, WebRequestAnswer& answer);
	bool executeSCHEMA( const char* path, const WebRequestContent& content, WebRequestAnswer& answer);
	bool executePostTransaction( void* self, int classid, const char* typenam, const char* contextnam, WebRequestAnswer& answer);
	bool executeDeclareConfiguration( const char* typenam, const char* contextnam, const char* request_method, bool init, const WebRequestContent& content, WebRequestAnswer& answer);
	bool executeDeleteConfiguration( const char* typenam, const char* contextnam, WebRequestAnswer& answer);
	bool executeCommitTransaction( const papuga_ValueVariant* obj, WebRequestAnswer& answer);
	bool initContentRequest( WebRequestAnswer& answer, const char* contextType, const char* schema);
	bool feedContentRequest( WebRequestAnswer& answer, const WebRequestContent& content);
	bool createEmptyRequestContext( WebRequestAnswer& answer, const char* className);
	bool createRequestContext( WebRequestAnswer& answer, const char* contextType, const char* contextName);
	bool initRequestContext( WebRequestAnswer& answer);
	bool inheritRequestContext( WebRequestAnswer& answer, const char* contextType, const char* contextName);
	bool debugContentRequest( WebRequestAnswer& answer);
	bool executeContentRequest( WebRequestAnswer& answer, const WebRequestContent& content);
	bool setResultContentType( WebRequestAnswer& answer, papuga_StringEncoding default_encoding, WebRequestContent::Type default_doctype);
	bool getContentRequestResult( WebRequestAnswer& answer, const WebRequestContent& content);
	bool callHostObjMethod( void* self, const papuga_RequestMethodDescription* methoddescr, const char* path, const WebRequestContent& content, papuga_CallResult& retval, WebRequestAnswer& answer);
	bool callHostObjMethod( void* self, const papuga_RequestMethodDescription* methoddescr, const char* path, const WebRequestContent& content, WebRequestAnswer& answer);
	bool callExtensionMethod( void* self, const papuga_RequestMethodDescription* methoddescr, papuga_RequestContext* context, const char* resultname, WebRequestAnswer& answer);
	bool executeMainSchema( const char* schema, const WebRequestContent& content, WebRequestAnswer& answer);
	bool executeContextSchema( const char* contextType, const char* contextName, const char* schema, const WebRequestContent& content, WebRequestAnswer& answer);
	bool executeContextSchema( papuga_RequestContext* context, const char* contextType, const char* schema, const WebRequestContent& content, WebRequestAnswer& answer);
	void releaseContext();
	bool executeSchemaDescriptionRequest( const char* method, const char* path, WebRequestAnswer& answer);

private:
	WebRequestHandler* m_handler;
	WebRequestLoggerInterface* m_logger;
	ConfigurationHandler* m_confighandler;
	papuga_RequestLogger m_callLogger;	//< request call logger (for papuga)
	TransactionPool* m_transactionPool;	//< transaction pool
	papuga_Allocator m_allocator;
	papuga_RequestContext* m_context;
	bool m_context_ownership;
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
	std::string m_html_base_href;
	char m_errbuf_mem[ 4096];
	char m_allocator_mem[ 1<<14];
};

}//namespace
#endif


