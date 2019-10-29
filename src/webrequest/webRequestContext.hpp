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
	enum MethodId {
		Method_Undefined,
		Method_GET,
		Method_PUT,
		Method_POST,
		Method_PATCH,
		Method_DELETE,
		Method_OPTIONS,
		Method_HEAD
	};
	static const char* methodIdName( const MethodId& m);
	static MethodId methodIdFromName( const char* methodname);

private:
	// Implemented in webRequestContext:
	/// \brief Execute a request of type ObjectRequest
	bool executeObjectRequest( const WebRequestContent& content);

	// Implemented in webRequestContext_obj:
	/// \brief Initialize the request logger for papuga
	void initCallLogger();
	/// \brief Create an empty object
 	bool initEmptyObject();
	/// \brief Create an object that inherits all from the root context (object loaded from main configuration)
	bool initRootObject();
	/// \brief Init object for schema description requests
	bool initSchemaDescriptionObject();
	/// \brief Init object for ordinary requests
	bool initRequestObject();
	/// \brief Reset object for requests
	void resetRequestObject();
	/// \brief Inherit all objects from a concept identified by type and name
	bool inheritRequestContext( const char* contextType_, const char* contextName_);
	/// \brief Inititialize request and result content types and character set encodings
	bool initContentType( const WebRequestContent& content);
	/// \brief Execute a request creating a transaction (POST)
	bool executePostTransaction();
	/// \brief Execute a request doing the commit (PUT) of a transaction
	bool executeCommitTransaction();

	// Implemented in webRequestContext_method:
	/// \brief Call a method and put the result to the answer of the request
	bool callHostObjMethodToAnswer( void* self, const papuga_RequestMethodDescription* methoddescr, const char* path, const WebRequestContent& content);
	/// \brief Call a method and assign the result to a variable of the current context
	bool callHostObjMethodToVariable( void* self, const papuga_RequestMethodDescription* methoddescr, PapugaContextRef& context_, const char* resultname);
		/// \brief Helper to log a method call
		void logMethodCall( const papuga_RequestMethodDescription* methoddescr);

	// Implemented in webRequestContext_schema:
	/// \brief Get the schema identifier specified by context
	SchemaId getSchemaId();
	/// \brief Get the schema identifier specified by arguments
	SchemaId getSchemaId( const char* contextType_, MethodId methodid_);
	/// \brief Get the schema identifier for a configuration update request
	static SchemaId getSchemaId_updateConfiguration( const char* contextType_);
	/// \brief Get the schema identifier for a configuration patch request
	static SchemaId getSchemaId_patchConfiguration( const char* contextType_);
	/// \brief Evaluate if a schema exists
	SchemaId getSchemaId_deleteConfiguration( const char* contextType_, const char* createConfigurationMethod);
	/// \brief Evaluate schema for schema description query
	SchemaId getSchemaId_description();
	/// \brief Evaluate if a schema exists
	bool hasContentSchemaAutomaton( const SchemaId& schemaid);
	/// \brief Initialization of the automaton for a schema request
	bool initContentSchemaAutomaton( const SchemaId& schemaid);
	/// \brief Execute the schema request initialized
	/// \param[in] content web request content
	bool executeContentSchemaAutomaton( const WebRequestContent& content);
		/// \brief Helper of executeContentSchemaAutomaton, initialize the request
		bool initContentSchemaRequest();
		/// \brief Helper of executeContentSchemaAutomaton, feed the content to the request
		/// \param[in] content web request content
		bool feedContentSchemaRequest( const WebRequestContent& content);
		/// \brief Do inherit context operations
		bool inheritContentSchemaRequestContext();
		/// \brief Execute calls of the request within the context 
		/// \param[in] content web request content
		bool executeContentSchemaCalls( const WebRequestContent& content);

	// Implemented in webRequestContext_env:
	/// \brief Execute assignments defined by environment in a schema request
	bool initSchemaEnvAssignments();

	// Implemented in webRequestContext_error:
	/// \brief Report an error in a schema request
	/// \param[in] content web request content
	void reportRequestError( const papuga_RequestError& reqerr, const WebRequestContent& content);

	// Implemented in webRequestContext_result:
	/// \brief Get a pointer to the result string or a delegate request content string of a schema request result
	/// \param[in] result structure of the request result or of a delegate request
	/// \param[out] resultlen length of returned result string in bytes
	/// \param[out] errcode in case of error, untouched else
	const char* getResultString( papuga_RequestResult* result, std::size_t& resultlen, papuga_ErrorCode& errcode);
	/// \brief Initialize the result of a content request
	bool getContentRequestResult();
	/// \brief Initialize the list of delegate requests of a content request
	/// \param[out] delegateRequests list of delegate requests
	bool getContentRequestDelegateRequests( std::vector<WebRequestDelegateRequest>& delegateRequests);
	/// \brief Evaluate if there are delegate requests in a content request
	bool hasContentRequestDelegateRequests() const;
	/// \brief Evaluate if there is a result in a content request
	bool hasContentRequestResult() const;
	/// \brief Transfer the current context to the handler as active object
	bool transferContext();
	/// \brief Define the current request to have failed
	void setAnswer( int errcode, const char* errstr=0, bool doCopy=false);
	/// \brief Define the answer of the current request to be a link (POST request result)
	bool setAnswerLink( const char* title, const std::string& lnk, int linklevel);

	// Implemented in webRequestContext_meta:
	/// \brief List variables of the object loaded
	bool executeListVariables();
	/// \brief Get the description of a schema (request with /schema prefix in the URL)
	bool executeSchemaDescription();
	/// \brief OPTIONS request
	bool executeOPTIONS();

	// Implemented in webRequestContext_config:
	/// \brief Load main configuration in the initialization phase of the service
	/// \param[in] content web request content
	bool loadMainConfiguration( const WebRequestContent& content);
	/// \brief Load a configuration embedded in the configuration file loaded in the initialization phase of the service
	bool loadEmbeddedConfiguration( const WebRequestContent& content);
	/// \brief Helper of loadConfigurationRequest or updateConfigurationRequest, initiate the configuration request
	/// \param[in] content web request content
	/// \param[in] update, true if the configuration is written to file as update, e.g. overwriting initial version, keeping its file name, or is written as a new file
	bool initConfigurationRequest( const WebRequestContent& content, bool update);
	/// \brief Load configuration request for a not yet existing object
	/// \param[in] content web request content
	bool loadConfigurationRequest( const WebRequestContent& content);
	/// \brief Update configuration request for an existing object
	/// \param[in] content web request content
	bool updateConfigurationRequest( const WebRequestContent& content);
	/// \brief Retry of an update configuration request with exclusive access on data, rejected before with ErrorCodeServiceNeedExclusiveAccess due to the exclusive flag set in the automaton definition
	/// \param[in] content web request content
	bool updateConfigurationRequest_retry( const WebRequestContent& content);
	/// \brief Delete configuration request
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
	MethodId m_methodId;			//< request method enum id
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


