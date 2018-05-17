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
#include "strus/lib/bindings_description.hpp"
#include "strus/lib/error.hpp"
#include "strus/bindingClasses.h"
#include "schemes.hpp"
#include "strus/webRequestLoggerInterface.hpp"
#include "strus/errorCodes.hpp"
#include "strus/base/local_ptr.hpp"
#include "strus/base/string_format.hpp"
#include "strus/base/fileio.hpp"
#include "papuga/request.h"
#include "papuga/errors.h"
#include "papuga/typedefs.h"
#include "papuga/valueVariant.hpp"
#include "papuga/valueVariant.h"
#include "papuga/constants.h"
#include "private/internationalization.hpp"
#include <vector>
#include <algorithm>
#include <cstring>
#include <cstdarg>
#include <ctime>

using namespace strus;

struct MethodDescription
{
	enum {MaxNofArgs=8};

	const char* request_method;
	papuga_RequestMethodDescription data;
	int args[ MaxNofArgs-1];

	MethodDescription(
		const char* request_method_,
		const papuga_RequestMethodId& id,
		int httpstatus_success, 
		const char* resulttype,
		const char* result_rootelem,
		const char* result_listelem,
		bool has_content,
		int nofParams,
		...)
	{
		va_list params;
		va_start( params, nofParams);

		if (nofParams >= MaxNofArgs) throw std::bad_alloc();
		int ai = 0, ae = nofParams;
		for (; ai != ae; ++ai)
		{
			args[ai] = (int)va_arg( params, int);
		}
		args[ai] = 0;
		request_method = request_method_;
		std::memcpy( &data.id, &id, sizeof( data.id));
		data.paramtypes = args;
		data.has_content = has_content;
		data.httpstatus_success = httpstatus_success;
		data.resulttype = resulttype;
		data.result_rootelem = result_rootelem;
		data.result_listelem = result_listelem;
		va_end( params);
	}

	void addToHandler( papuga_RequestHandler* handler) const
	{
		if (!papuga_RequestHandler_add_method( handler, request_method, &data)) throw std::bad_alloc();
	}
};

struct IntrospectionMethodDescription
	:public MethodDescription
{
	IntrospectionMethodDescription( const papuga_RequestMethodId& id, const char* rootelem)
		:MethodDescription( "GET", id, 200, NULL, rootelem, "value", false/*has content*/, 1, WebRequestHandler::ParamPathArray){}
};
struct TransformationMethodDescription
	:public MethodDescription
{
	TransformationMethodDescription( const papuga_RequestMethodId& id, const char* rootelem)
		:MethodDescription( "GET", id, 200, NULL, rootelem, "value", true/*has content*/, 1, WebRequestHandler::ParamContent){}
};
struct InsertMethodDescription
	:public MethodDescription
{
	InsertMethodDescription( const papuga_RequestMethodId& id)
		:MethodDescription( "PUT", id, 204/*no content*/, NULL, "insert", "value", true/*has content*/, 2, WebRequestHandler::ParamPathString, WebRequestHandler::ParamContent){}
};
struct CommitTransactionMethodDescription
	:public MethodDescription
{
	CommitTransactionMethodDescription( const papuga_RequestMethodId& id)
		:MethodDescription( "PUT/transaction", id, 204/*no content*/, NULL, "commit", "value", false/*has content*/, 0){}
};
struct PostTransactionMethodDescription
	:public MethodDescription
{
	PostTransactionMethodDescription( const papuga_RequestMethodId& id, const char* rootelem)
		:MethodDescription( "POST/transaction", id, 200, NULL, rootelem, "id", false/*has content*/, 0){}
};

template <class SCHEME>
class DefineScheme
	:public SCHEME
{
public:
	explicit DefineScheme( const char* contextType)
		:m_contextType(contextType){}

	void addToHandler( papuga_RequestHandler* handler, const char* schemeName) const
	{
		if (!papuga_RequestHandler_add_scheme( handler, m_contextType, schemeName, papuga::RequestAutomaton::impl())) throw std::bad_alloc();\
	}

private:
	const char* m_contextType;
};

template <class SCHEME>
class DefineMainScheme
	:public DefineScheme<SCHEME>
{
public:
	DefineMainScheme() :DefineScheme<SCHEME>(""){}
};

template <class SCHEME>
class DefineConfigScheme
	:public DefineScheme<SCHEME>
{
public:
	DefineConfigScheme() :DefineScheme<SCHEME>( ROOT_CONTEXT_NAME){}
};

static const char* g_context_typenames[] = {"storage","docanalyzer","queryanalyzer","inserter",0};

WebRequestHandler::WebRequestHandler(
		WebRequestLoggerInterface* logger_,
		const std::string& html_head_,
		const std::string& config_store_dir_,
		const std::string& configstr_,
		int maxIdleTime_,
		int nofTransactionsPerSeconds)
	:m_debug_maxdepth(logger_?logger_->structDepth():0)
	,m_logger(logger_)
	,m_impl(0)
	,m_configHandler(logger_,config_store_dir_,g_context_typenames)
	,m_html_head(html_head_)
	,m_config_store_dir(config_store_dir_)
	,m_transactionPool( ::time(NULL), maxIdleTime_*2, nofTransactionsPerSeconds, logger_)
	,m_maxIdleTime(maxIdleTime_)
	,m_ticker(this,std::max( maxIdleTime_/20, 10))
{
	m_impl = papuga_create_RequestHandler( strus_getBindingsClassDefs());
	if (!m_impl) throw std::bad_alloc();

	using namespace strus::webrequest;

	try
	{
		namespace mt = strus::bindings::method;

		// [1] Add schemes
		static const DefineMainScheme<Scheme_INIT_Context> scheme_INIT_Context;
		scheme_INIT_Context.addToHandler( m_impl, ROOT_CONTEXT_NAME/*scheme name*/);

		static const DefineConfigScheme<Scheme_Context_INIT_Storage> scheme_Context_INIT_Storage;
		scheme_Context_INIT_Storage.addToHandler( m_impl, "storage");
		static const DefineConfigScheme<Scheme_Context_PUT_Storage> scheme_Context_PUT_Storage;
		scheme_Context_PUT_Storage.addToHandler( m_impl, "PUT/storage");
		static const DefineConfigScheme<Scheme_Context_DELETE_Storage> scheme_Context_DELETE_Storage;
		scheme_Context_DELETE_Storage.addToHandler( m_impl, "DELETE/storage");

		static const DefineConfigScheme<Scheme_Context_PUT_DocumentAnalyzer> scheme_Context_PUT_DocumentAnalyzer;
		scheme_Context_PUT_DocumentAnalyzer.addToHandler( m_impl, "docanalyzer");
		scheme_Context_PUT_DocumentAnalyzer.addToHandler( m_impl, "PUT/docanalyzer");

		static const DefineConfigScheme<Scheme_Context_PUT_Inserter> scheme_Context_PUT_Inserter;
		scheme_Context_PUT_Inserter.addToHandler( m_impl, "inserter");
		scheme_Context_PUT_Inserter.addToHandler( m_impl, "PUT/inserter");

		// [2] Add methods
		static const IntrospectionMethodDescription mt_Context_GET( mt::Context::introspection(), "config");
		mt_Context_GET.addToHandler( m_impl);

		static const IntrospectionMethodDescription mt_DocumentAnalyzer_GET( mt::DocumentAnalyzer::introspection(), "analyzer");
		mt_DocumentAnalyzer_GET.addToHandler( m_impl);

		static const TransformationMethodDescription mt_DocumentAnalyzer_GET_content( mt::DocumentAnalyzer::analyzeMultiPart(), "doc");
		mt_DocumentAnalyzer_GET_content.addToHandler( m_impl);

		static const IntrospectionMethodDescription mt_QueryAnalyzer_GET( mt::QueryAnalyzer::introspection(), "analyzer");
		mt_QueryAnalyzer_GET.addToHandler( m_impl);

		static const IntrospectionMethodDescription mt_StorageClient_GET( mt::StorageClient::introspection(), "storage");
		mt_StorageClient_GET.addToHandler( m_impl);
		static const PostTransactionMethodDescription mt_StorageClient_POST_transaction( mt::StorageClient::createTransaction(), "transaction");
		mt_StorageClient_POST_transaction.addToHandler( m_impl);

		static const IntrospectionMethodDescription mt_Inserter_GET( mt::Inserter::introspection(), "inserter");
		mt_Inserter_GET.addToHandler( m_impl);
		static const PostTransactionMethodDescription mt_Inserter_POST_transaction( mt::Inserter::createTransaction(), "transaction");
		mt_Inserter_POST_transaction.addToHandler( m_impl);

		static const InsertMethodDescription mt_InserterTransaction_PUT( mt::InserterTransaction::insertDocument());
		mt_InserterTransaction_PUT.addToHandler( m_impl);
		static const CommitTransactionMethodDescription mt_InserterTransaction_COMMIT( mt::InserterTransaction::commit());
		mt_InserterTransaction_COMMIT.addToHandler( m_impl);

		loadConfiguration( configstr_);
		m_configHandler.clearUnfinishedTransactions();
		m_configHandler.deleteObsoleteConfigurations();
		if (!m_ticker.start()) throw std::bad_alloc();
	}
	catch (const std::bad_alloc&)
	{
		clear();
		throw std::bad_alloc();
	}
	catch (const std::runtime_error& err)
	{
		clear();
		throw err;
	}
}

void WebRequestHandler::clear()
{
	if (m_impl) {papuga_destroy_RequestHandler( m_impl); m_impl=0;}
}
	

WebRequestHandler::~WebRequestHandler()
{
	m_ticker.stop();
	clear();
}

static void setStatus( WebRequestAnswer& status, ErrorCode errcode, const char* errmsg=0)
{
	const char* errstr = errorCodeToString( errcode);
	int httpstatus = errorCodeToHttpStatus( errcode);
	if (errmsg)
	{
		char errbuf[ 1024];
		if ((int)sizeof(errbuf)-1 >= std::snprintf( errbuf, sizeof(errbuf), "%s, %s", errmsg, errstr))
		{
			errbuf[ sizeof(errbuf)-1] = 0;
		}
		status.setError( httpstatus, errcode, errbuf, true);
	}
	else
	{
		status.setError( httpstatus, errcode, errstr);
	}
}

WebRequestContextInterface* WebRequestHandler::createContext(
		const char* accepted_charset,
		const char* accepted_doctype,
		const char* html_base_href,
		WebRequestAnswer& status)
{
	try
	{
		return new WebRequestContext( this, m_logger, &m_configHandler, &m_transactionPool, accepted_charset, accepted_doctype, html_base_href);
	}
	catch (const std::bad_alloc&)
	{
		setStatus( status, ErrorCodeOutOfMem);
	}
	catch (...)
	{
		setStatus( status, ErrorCodeUncaughtException);
	}
	return NULL;
}

void WebRequestHandler::tick()
{
	m_transactionPool.collectGarbage( ::time(NULL));
}

void WebRequestHandler::loadConfiguration( const std::string& configstr)
{
	const char* config_doctype = "json";
	const char* config_charset = "utf-8";
	const char* accepted_charset = "UTF-8";
	const char* accepted_doctype = "application/json";
	WebRequestAnswer status;

	if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogConfiguration))
	{
		m_logger->logPutConfiguration( ROOT_CONTEXT_NAME, ROOT_CONTEXT_NAME, configstr);
	}
	strus::local_ptr<WebRequestContextInterface> ctx( createContext( accepted_charset, accepted_doctype, ""/*html_base_href*/, status));
	WebRequestContextInterface* ctxi = ctx.get();
	if (!ctxi) throw std::runtime_error( status.errorstr() ? status.errorstr() : _TXT("unknown error"));

	// Load main configuration:
	strus::WebRequestContent content( config_charset, config_doctype, configstr.c_str(), configstr.size());
	if (!ctxi->executeLoadMainConfiguration( content, status))
	{
		throw std::runtime_error( status.errorstr() ? status.errorstr() : _TXT("unknown error"));
	}
	// Load sub configurations:
	std::vector<ConfigurationDescription> cfglist = m_configHandler.getSubConfigurations( configstr);
	std::vector<ConfigurationDescription>::const_iterator ci = cfglist.begin(), ce = cfglist.end();
	for (; ci != ce; ++ci)
	{
		strus::WebRequestContent subcontent( config_charset, ci->doctype.c_str(), ci->contentbuf.c_str(), ci->contentbuf.size());
		if (!ctxi->executeLoadSubConfiguration( ci->type.c_str(), ci->name.c_str(), subcontent, status))
		{
			throw strus::runtime_error(
				_TXT("error loading sub configuration %s '%s': %s"),
				ci->type.c_str(), ci->name.c_str(), status.errorstr());
		}
		m_configHandler.declareSubConfiguration( ci->type.c_str(), ci->name.c_str());
	}
	// Load put configurations:
	cfglist = m_configHandler.getStoredConfigurations();
	ci = cfglist.begin(), ce = cfglist.end();
	for (; ci != ce; ++ci)
	{
		strus::WebRequestContent subcontent( config_charset, ci->doctype.c_str(), ci->contentbuf.c_str(), ci->contentbuf.size());
		if (!ctxi->executeLoadSubConfiguration( ci->type.c_str(), ci->name.c_str(), subcontent, status))
		{
			throw strus::runtime_error(
				_TXT("error loading stored sub configuration %s '%s': %s"),
				ci->type.c_str(), ci->name.c_str(), status.errorstr());
		}
		m_configHandler.declareSubConfiguration( ci->type.c_str(), ci->name.c_str());
	}
}

bool WebRequestHandler::transferContext(
		const char* contextType,
		const char* contextName,
		papuga_RequestContext* context,
		WebRequestAnswer& status)
{
	papuga_ErrorCode errcode = papuga_Ok;
	strus::unique_lock lock( m_mutex_context_transfer);
	if (!papuga_RequestHandler_transfer_context( m_impl, contextType, contextName, context, &errcode))
	{
		setStatus( status, papugaErrorToErrorCode( errcode));
		return false;
	}
	return true;
}

bool WebRequestHandler::removeContext(
		const char* contextType,
		const char* contextName,
		WebRequestAnswer& status)
{
	papuga_ErrorCode errcode = papuga_Ok;
	strus::unique_lock lock( m_mutex_context_transfer);
	if (!papuga_RequestHandler_remove_context( m_impl, contextType, contextName, &errcode))
	{
		setStatus( status, papugaErrorToErrorCode( errcode));
		return false;
	}
	return true;
}



