/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Implementation of the interface for executing XML/JSON requests on the strus bindings
/// \file "webRequestHandler.hpp"
#ifndef _STRUS_WEB_REQUEST_HANDLER_IMPL_HPP_INCLUDED
#define _STRUS_WEB_REQUEST_HANDLER_IMPL_HPP_INCLUDED
#include "strus/webRequestHandlerInterface.hpp"
#include "strus/base/thread.hpp"
#include "papuga/requestHandler.h"
#include "papuga/requestLogger.h"
#include <cstddef>
#include <utility>
#include <set>

namespace strus
{
/// \brief Forward declaration
class WebRequestLoggerInterface;
/// \brief Forward declaration
class WebRequestContext;

/// \brief Implementation of the interface for executing XML/JSON requests on the strus bindings
class WebRequestHandler
	:public WebRequestHandlerInterface
{
public:
	WebRequestHandler( 
			WebRequestLoggerInterface* logger_,
			const std::string& html_head_,
			const std::string& config_store_dir_,
			const std::string& configstr_);
	virtual ~WebRequestHandler();

	virtual WebRequestContextInterface* createContext(
			const char* accepted_charset,
			const char* accepted_doctype,
			WebRequestAnswer& status) const;

public:/*WebRequestContext*/
	const papuga_RequestHandler* impl() const	{return m_impl;}
	papuga_RequestLogger* call_logger()		{return &m_call_logger;}
	const char* html_head() const			{return m_html_head.c_str();}
	int debug_maxdepth() const			{return m_debug_maxdepth;}

	bool loadConfiguration(
			const char* contextType,
			const char* contextName,
			const char* scheme,
			bool storedForReload,
			const WebRequestContent& content,
			WebRequestAnswer& status);

private:
	struct ConfigurationTransaction
	{
		std::string failed_filename;
		std::string filename;
	};

	bool storeConfiguration(
			ConfigurationTransaction& transaction,
			const char* contextType,
			const char* contextName,
			const char* scheme,
			const WebRequestContent& content,
			WebRequestAnswer& status) const;
	bool commitStoreConfiguration(
			const ConfigurationTransaction& transaction,
			WebRequestAnswer& status) const;

public:/*WebRequestContext: Get methods to execute*/
	struct MethodDescription
	{
		enum ParamType {ParamEnd=0,ParamPathString,ParamPathArray,ParamDocumentClass,ParamContent};

		papuga_RequestMethodId mid;
		const ParamType* params;

		MethodDescription( const papuga_RequestMethodId mid_, const ParamType* params_)
			:params(params_){mid.classid=mid_.classid;mid.functionid=mid_.functionid;}
	};

	static const MethodDescription* getListMethod( int classid);
	static const MethodDescription* getViewMethod( int classid);
	static const MethodDescription* getPostDocumentMethod( int classid);
	static const MethodDescription* getPutDocumentMethod( int classid);
	static const MethodDescription* getDeleteMethod( int classid);
	static const MethodDescription* getPatchMethod( int classid);

private:
	WebRequestContext* createContext_( const char* accepted_charset, const char* accepted_doctype, WebRequestAnswer& status) const;

	/// \brief Add a scheme to the handler
	void addScheme( const char* type, const char* name, const papuga_RequestAutomaton* automaton);

	void loadConfiguration( const std::string& configstr);
	bool loadStoredConfigurations();

private:
	mutable strus::mutex m_mutex;		//< mutex for locking mutual exclusion of configuration requests
	mutable int m_config_counter;		//< counter to order configurations stored that have the same date
	int m_debug_maxdepth;			//< maximum depth for debug structures
	WebRequestLoggerInterface* m_logger;	//< request logger 
	papuga_RequestLogger m_call_logger;	//< request call logger (for papuga)
	papuga_RequestHandler* m_impl;		//< request handler
	std::string m_html_head;		//< header include for HTML output (for stylesheets, meta data etc.)
	std::string m_config_store_dir;		//< directory where to store configurations loaded as request
	char const** m_schemes;			//< NULL terminated list of schemes available */
	int m_nofschemes;			//< number of elements in m_schemes
	char const** m_context_schemes;		//< NULL terminated list of schemes of the main context available */
	int m_nofcontext_schemes;		//< number of elements in m_context_schemes
};

}//namespace
#endif


