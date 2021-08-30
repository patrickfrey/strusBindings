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
#include "pathBuf.hpp"
#include "transaction.hpp"
#include "papuga/requestContext.h"
#include "papuga/requestParser.h"
#include "papuga/typedefs.h"
#include "papuga/allocator.h"
#include "papugaContextRef.hpp"
#include <stdexcept>

#define STRUS_LIST_ROOT_ELEMENT "list"

namespace strus
{

/// \brief Forward declaration
class WebRequestHandler;

/// \brief Implementation of the interface for executing an XML/JSON request on the strus bindings
class WebRequestContext
	:public WebRequestContextInterface
{
public:
	/// \brief Constructor for web request
	WebRequestContext(
		WebRequestHandler* handler_,
		WebRequestLoggerInterface* logger_,
		TransactionPool* transactionPool_,
		const char* accepted_charset_,
		const char* accepted_doctype_,
		const char* html_base_href_,
		const char* method_,
		const char* path_);

	virtual ~WebRequestContext();

	virtual bool execute( const WebRequestContent& content);

	virtual std::vector<WebRequestDelegateRequest> getDelegateRequests();

	virtual bool putDelegateRequestAnswer(
			const char* schema,
			const WebRequestAnswer& answer);

	virtual bool complete();
	virtual WebRequestAnswer getAnswer() const;

	enum {
		MaxLogContentSize=2048 /*2K*/,
		MaxLogItemSize=48
	};

private:
	enum RequestType {
		UndefinedRequest,
		SchemaDescriptionRequest,
		MethodOptionsRequest,
		ObjectRequest,
		InterruptedLoadConfigurationRequest
	};
	struct SchemaId
	{
		const char* contextType;
		const char* schemaName;

		SchemaId()
			:contextType(0),schemaName(0){}
		SchemaId( const char* contextType_, const char* schemaName_)
			:contextType(contextType_),schemaName(schemaName_){}
		SchemaId( const SchemaId& o)
			:contextType(o.contextType),schemaName(o.schemaName){}
		bool defined() const
		{
			return contextType && schemaName;
		}
	};

public:/*WebRequestContext*/
	static papuga_StringEncoding defaultEncoding() {return papuga_UTF8;}
	static WebRequestContent::Type defaultDocType() {return WebRequestContent::HTML;}

private:
	/// \brief Clone constructor
	WebRequestContext(
		WebRequestHandler* handler_,
		WebRequestLoggerInterface* logger_,
		TransactionPool* transactionPool_,
		const char* method_,
		const char* contextType_,
		const char* contextName_,
		const PapugaContextRef& context_,
		const RequestType& requestType_);

	/// \brief Constructor of a clone with a reset state for the execution of schemas to process the delegate request results
	WebRequestContext* createClone( const RequestType& requestType_) const;

private:
	// Implemented in webRequestContext:
	/// \brief Execute a request of type ObjectRequest
	bool executeObjectRequest( const WebRequestContent& content);
	/// \brief Fetch all info/debug trace messages from current context
	std::string fetchContextInfoMessages();

	// Implemented in webRequestContext_obj:
	/// \brief Create an empty object
 	bool initEmptyObject();
	/// \brief Create an object that inherits all from the root context (object loaded from main configuration)
	bool initRootObject();
	/// \brief Init object for ordinary requests
	bool initRequestObject();
	/// \brief Reset object for requests
	void resetRequestObject();
	/// \brief Inititialize request and result content types and character set encodings
	bool initContentType( const WebRequestContent& content);
		/// \brief Inititialize root element of the content
		/// \note Called by init content type
		bool initContentRootElement( const WebRequestContent& content);
		/// \brief Inititialize result content type and character set encoding
		/// \note Called by init content type
		bool initResultContentType();

	// Implemented in webRequestContext_result:
	/// \brief Transfer the current context to the handler as active object
	bool transferContext();
	/// \brief Define the current request to have failed
	void setAnswer( int errcode, const char* errstr=0, bool doCopy=false);
	/// \brief Define the answer of the current request to be a link (POST request result)
	bool setAnswerLink( const char* title, const std::string& lnk, int linklevel);

	// Implemented in webRequestContext_meta:
	/// \brief List variables of the object loaded
	bool executeListVariables();
	/// \brief List schema names
	bool executeListSchemas();
	/// \brief Get the description of a schema (request with /schema prefix in the URL)
	bool executeSchemaDescription( const char* name);
	/// \brief OPTIONS request
	bool executeOPTIONS();

private:
	WebRequestHandler* m_handler;		//< request handler creating this request
	WebRequestLoggerInterface* m_logger;	//< logger for logging this request
	int m_logMask;				//< what to log
	TransactionPool* m_transactionPool;	//< transaction pool
	TransactionRef m_transactionRef;	//< transaction reference
	papuga_Allocator m_allocator;		//< papuga allocator used for this request context
	RequestType m_requestType;		//< classification of this request
	char m_requestMethod[8];		//< request method
	const char* m_contextType;		//< context type
	const char* m_contextName;		//< context name
	const char* m_rootElement;		//< root element of the request
	PapugaContextRef m_context;		//< context reference
	const papuga_ValueVariant* m_obj;	//< pointer to object in context selected
	PathBuf m_path;				//< iterator on path of the request
	papuga_StringEncoding m_encoding;	//< character set encoding of the request
	papuga_ContentType m_doctype;		//< document class of the request
	const char* m_doctypestr;		//< document class of the request as string
	papuga_StringEncoding m_result_encoding;//< character set encoding of the result
	WebRequestContent::Type m_result_doctype;//< document class of the result
	papuga_ErrorBuffer m_errbuf;		//< error buffer for papuga
	WebRequestAnswer m_answer;		//< answer of the request
	const char* m_accepted_charset;		//< accepted character set HTTP field
	const char* m_accepted_doctype;		//< accepted doctype HTTP field
	std::string m_html_base_href;		//< link address base for HTML
	char m_errbuf_mem[ 4096];		//< memory used by error buffer for papuga
	char m_allocator_mem[ 1<<14];		//< memory used by papuga allocator used for this request context
};

}//namespace
#endif


