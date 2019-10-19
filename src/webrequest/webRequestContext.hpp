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
#include "pathBuf.hpp"
#include "transaction.hpp"
#include "papuga/requestHandler.h"
#include "papuga/requestParser.h"
#include "papuga/requestResult.h"
#include "papuga/request.h"
#include "papuga/typedefs.h"
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
		ConfigurationHandler* configHandler_,
		TransactionPool* transactionPool_,
		const char* accepted_charset_,
		const char* accepted_doctype_,
		const char* html_base_href_,
		const char* method_,
		const char* path_);

	/// \brief Constructor for configuration request
	WebRequestContext(
		WebRequestHandler* handler_,
		WebRequestLoggerInterface* logger_,
		ConfigurationHandler* configHandler_,
		TransactionPool* transactionPool_,
		const char* contextType_,
		const char* contextName_);

	virtual ~WebRequestContext();

	virtual bool execute(
			const WebRequestContent& content);

	virtual std::vector<WebRequestDelegateRequest> getDelegateRequests();

	virtual bool putDelegateRequestAnswer(
			const char* schema,
			const WebRequestContent& content);

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
		LoadMainConfiguration,
		LoadEmbeddedConfiguration,
		ObjectRequest
	};
	struct SchemaId
	{
		const char* contextType;
		const char* schemaName;

		SchemaId()
			:contextType(""),schemaName(ROOT_CONTEXT_NAME){}
		SchemaId( const char* contextType_, const char* schemaName_)
			:contextType(contextType_),schemaName(schemaName_){}
		SchemaId( const SchemaId& o)
			:contextType(o.contextType),schemaName(o.schemaName){}
	};

public:/*WebRequestContext*/
	static papuga_StringEncoding defaultEncoding() {return papuga_UTF8;}
	static WebRequestContent::Type defaultDocType() {return WebRequestContent::HTML;}
	static RequestType configRequestType( const char* contextType);

private:
	/// \brief Clone constructor
	WebRequestContext(
		WebRequestHandler* handler_,
		WebRequestLoggerInterface* logger_,
		ConfigurationHandler* configHandler_,
		TransactionPool* transactionPool_,
		const char* contextType_,
		const char* contextName_,
		const PapugaContextRef& context_,
		const RequestType& requestType_);

	/// \brief Constructor of a clone with a reset state for the execution of schemas to process the delegate request results
	WebRequestContext* createClone( const RequestType& requestType_) const;

private:
	// Implemented in webRequestContext:
	bool executeObjectRequest( const WebRequestContent& content);

	// Implemented in webRequestContext_obj:
	void initCallLogger();
	bool initEmptyObject();
	bool initRootObject();
	bool initSchemaDescriptionObject();
	bool initRequestObject();
	void resetRequestObject();
	bool inheritRequestContext( const char* contextType_, const char* contextName_);
	bool initContentType( const WebRequestContent& content);
	bool executePostTransaction();
	bool executeCommitTransaction();

	// Implemented in webRequestContext_method:
	bool callHostObjMethodToAnswer( void* self, const papuga_RequestMethodDescription* methoddescr, const char* path, const WebRequestContent& content);
	bool callHostObjMethodToVariable( void* self, const papuga_RequestMethodDescription* methoddescr, PapugaContextRef& context_, const char* resultname);
		void logMethodCall( const papuga_RequestMethodDescription* methoddescr);

	// Implemented in webRequestContext_schema:
	SchemaId getSchemaId();
	SchemaId getSchemaId( const char* contextType_, const char* method_);
	bool executeContentSchemaRequest( const SchemaId& schemaid, const WebRequestContent& content);
		bool initContentSchemaAutomaton( const SchemaId& schemaid);
		bool initContentSchemaRequest();
		bool feedContentSchemaRequest( const WebRequestContent& content);
		bool inheritContentSchemaRequestContext();
		bool executeContentSchema( const WebRequestContent& content);

	// Implemented in webRequestContext_env:
	bool initSchemaEnvAssignments();

	// Implemented in webRequestContext_error:
	void reportRequestError( const papuga_RequestError& reqerr, const WebRequestContent& content);

	// Implemented in webRequestContext_result:
	const char* getResultString( papuga_RequestResult* result, std::size_t& resultlen, papuga_ErrorCode& errcode);
	bool getContentRequestResult();
	bool getContentRequestDelegateRequests( std::vector<WebRequestDelegateRequest>& delegateRequests);
	bool hasContentRequestDelegateRequests() const;
	bool hasContentRequestResult() const;
	bool transferContext();
	void setAnswer( int errcode, const char* errstr=0, bool doCopy=false);
	bool setAnswerLink( const char* title, const std::string& lnk, int linklevel);

	// Implemented in webRequestContext_meta:
	bool executeListVariables();
	bool executeSchemaDescription();
	bool executeOPTIONS();

	// Implemented in webRequestContext_config:
	bool loadMainConfiguration( const WebRequestContent& content);
	bool loadEmbeddedConfiguration( const WebRequestContent& content);
	bool loadConfigurationRequest( const WebRequestContent& content);
	bool deleteConfigurationRequest();

private:
	WebRequestHandler* m_handler;		//< request handler creating this request
	WebRequestLoggerInterface* m_logger;	//< logger for logging this request
	int m_logMask;				//< what to log
	ConfigurationHandler* m_configHandler;	//< handler to manage configuration transactions
	ConfigurationTransaction m_configTransaction; //< current configuration transaction in case of a configuration request
	papuga_RequestLogger m_callLogger;	//< request call logger (for papuga)
	TransactionPool* m_transactionPool;	//< transaction pool
	TransactionRef m_transactionRef;	//< transaction reference
	papuga_Allocator m_allocator;		//< papuga allocator used for this request context
	RequestType m_requestType;		//< classification of this request
	const char* m_contextType;		//< context type
	const char* m_contextName;		//< context name
	PapugaContextRef m_context;		//< context reference
	const papuga_ValueVariant* m_obj;	//< pointer to object in context selected
	papuga_Request* m_request;		//< request data for papuga
	const char* m_method;			//< request method
	PathBuf m_path;				//< iterator on path of the request
	papuga_StringEncoding m_encoding;	//< character set encoding of the request
	papuga_ContentType m_doctype;		//< document class of the request
	const char* m_doctypestr;		//< document class of the request as string
	const papuga_RequestAutomaton* m_atm;	//< automaton of the schema to execute
	papuga_StringEncoding m_result_encoding;//< character set encoding of the result
	WebRequestContent::Type m_result_doctype;//< document class of the result
	papuga_RequestResult* m_results;	//< list of results or delegate request templates
	int m_nofResults;			//< list of result templates
	int m_resultIdx;			//< index of current result template
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


