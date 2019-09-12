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
#include "configurationUpdateRequest.hpp"
#include "strus/lib/bindings_description.hpp"
#include "strus/lib/error.hpp"
#include "strus/bindingClasses.h"
#include "schemas.hpp"
#include "strus/webRequestLoggerInterface.hpp"
#include "strus/webRequestDelegateContextInterface.hpp"
#include "strus/errorCodes.hpp"
#include "strus/base/local_ptr.hpp"
#include "strus/base/string_format.hpp"
#include "strus/base/string_conv.hpp"
#include "strus/base/shared_ptr.hpp"
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

static const char* g_config_doctype = "json";
static const char* g_config_charset = "utf-8";
static const char* g_config_accepted_charset = "UTF-8";
static const char* g_config_accepted_doctype = "application/json";

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

struct DumpMethodDescription
	:public MethodDescription
{
	DumpMethodDescription( const papuga_RequestMethodId& id, const char* rootelem)
		:MethodDescription( "GET", id, 200, NULL, rootelem, "value", false/*has content*/, 0){}
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

template <class SCHEMA>
class DefineSchema
	:public SCHEMA
{
public:
	explicit DefineSchema( const char* contextType)
		:m_contextType(contextType){}

	void addToHandler( papuga_RequestHandler* handler, const char* schemaName) const
	{
		if (!papuga_RequestHandler_add_schema( handler, m_contextType, schemaName, papuga::RequestAutomaton::impl(), papuga::RequestAutomaton::description())) throw std::bad_alloc();\
	}

private:
	const char* m_contextType;
};

template <class SCHEMA>
class DefineMainSchema
	:public DefineSchema<SCHEMA>
{
public:
	DefineMainSchema() :DefineSchema<SCHEMA>(""){}
};

template <class SCHEMA>
class DefineConfigSchema
	:public DefineSchema<SCHEMA>
{
public:
	DefineConfigSchema() :DefineSchema<SCHEMA>( ROOT_CONTEXT_NAME){}
};

static const char* g_context_typenames[] = {"contentstats","statserver","storage","docanalyzer","qryanalyzer","qryeval","inserter",0};

static void tickerFunction( void* THIS)
{
	((WebRequestHandler*)THIS)->tick();
}


WebRequestHandler::WebRequestHandler(
		WebRequestEventLoopInterface* eventLoop_,
		WebRequestLoggerInterface* logger_,
		const std::string& html_head_,
		const std::string& config_store_dir_,
		const std::string& service_name_,
		const std::string& configstr_,
		int maxIdleTime_,
		int nofTransactionsPerSeconds)
	:m_debug_maxdepth(logger_?logger_->structDepth():0)
	,m_logger(logger_)
	,m_impl(0)
	,m_configHandler(logger_,config_store_dir_,service_name_,g_context_typenames)
	,m_html_head(html_head_)
	,m_transactionPool( eventLoop_->time(), maxIdleTime_*2, nofTransactionsPerSeconds, logger_)
	,m_maxIdleTime(maxIdleTime_)
	,m_eventLoop( eventLoop_)
{
	m_impl = papuga_create_RequestHandler( strus_getBindingsClassDefs());
	if (!m_impl || !m_eventLoop->addTickerEvent( this, &tickerFunction)) throw std::bad_alloc();

	using namespace strus::webrequest;
	try
	{
		namespace mt = strus::bindings::method;

		// [1] Add schemas
		static const DefineMainSchema<Schema_INIT_Context> schema_INIT_Context;
		schema_INIT_Context.addToHandler( m_impl, ROOT_CONTEXT_NAME/*schema name*/);

		static const DefineConfigSchema<Schema_Context_CREATE_Storage> schema_Context_CREATE_Storage;
		schema_Context_CREATE_Storage.addToHandler( m_impl, "POST/storage");
		static const DefineConfigSchema<Schema_Context_DELETE_Storage> schema_Context_DELETE_Storage;
		schema_Context_DELETE_Storage.addToHandler( m_impl, "DELETE/storage");
		static const DefineConfigSchema<Schema_Context_INIT_Storage> schema_Context_INIT_Storage;
		schema_Context_INIT_Storage.addToHandler( m_impl, "PUT/storage");
		static const DefineSchema<Schema_StorageTransaction_PUT> schema_StorageTransaction_PUT("transaction/storage");
		schema_StorageTransaction_PUT.addToHandler( m_impl, "PUT");

		static const DefineConfigSchema<Schema_Context_CREATE_VectorStorage> schema_Context_CREATE_VectorStorage;
		schema_Context_CREATE_VectorStorage.addToHandler( m_impl, "POST/vstorage");
		static const DefineConfigSchema<Schema_Context_DELETE_VectorStorage> schema_Context_DELETE_VectorStorage;
		schema_Context_DELETE_VectorStorage.addToHandler( m_impl, "DELETE/vstorage");
		static const DefineConfigSchema<Schema_Context_INIT_VectorStorage> schema_Context_INIT_VectorStorage;
		schema_Context_INIT_VectorStorage.addToHandler( m_impl, "PUT/vstorage");
		static const DefineSchema<Schema_Context_PUT_VectorStorageTransaction> schema_Context_PUT_VectorStorageTransaction("transaction/vstorage");
		schema_Context_PUT_VectorStorageTransaction.addToHandler( m_impl, "PUT");

		static const DefineConfigSchema<Schema_Context_PUT_DocumentAnalyzer> schema_Context_INIT_DocumentAnalyzer;
		schema_Context_INIT_DocumentAnalyzer.addToHandler( m_impl, "POST/docanalyzer");
		static const DefineConfigSchema<Schema_Context_PUT_DocumentAnalyzer> schema_Context_PUT_DocumentAnalyzer;
		schema_Context_PUT_DocumentAnalyzer.addToHandler( m_impl, "PUT/docanalyzer");

		static const DefineConfigSchema<Schema_Context_INIT_QueryAnalyzer> schema_Context_INIT_QueryAnalyzer;
		schema_Context_INIT_QueryAnalyzer.addToHandler( m_impl, "POST/qryanalyzer");
		static const DefineConfigSchema<Schema_Context_PUT_QueryAnalyzer> schema_Context_PUT_QueryAnalyzer;
		schema_Context_PUT_QueryAnalyzer.addToHandler( m_impl, "PUT/qryanalyzer");

		static const DefineConfigSchema<Schema_Context_INIT_QueryEval> schema_Context_INIT_QueryEval;
		schema_Context_INIT_QueryEval.addToHandler( m_impl, "POST/qryeval");
		static const DefineConfigSchema<Schema_Context_PUT_QueryEval> schema_Context_PUT_QueryEval;
		schema_Context_PUT_QueryEval.addToHandler( m_impl, "PUT/qryeval");

		static const DefineConfigSchema<Schema_Context_PUT_StatisticsServer> schema_Context_PUT_StatisticsServer;
		schema_Context_PUT_StatisticsServer.addToHandler( m_impl, "PUT/statserver");
		schema_Context_PUT_StatisticsServer.addToHandler( m_impl, "POST/statserver");
		static const DefineSchema<Schema_StatisticsServer_PUT_statistics> schema_StatisticsServer_PUT_statistics_statistics("statserver");
		schema_StatisticsServer_PUT_statistics_statistics.addToHandler( m_impl, "PUT~statistics");

		static const DefineConfigSchema<Schema_Context_PUT_Inserter> schema_Context_PUT_Inserter;
		schema_Context_PUT_Inserter.addToHandler( m_impl, "PUT/inserter");
		schema_Context_PUT_Inserter.addToHandler( m_impl, "POST/inserter");

		static const DefineConfigSchema<Schema_Context_PUT_ContentStatistics> schema_Context_PUT_ContentStatistics;
		schema_Context_PUT_ContentStatistics.addToHandler( m_impl, "PUT/contentstats");
		schema_Context_PUT_ContentStatistics.addToHandler( m_impl, "POST/contentstats");

		static const DefineSchema<Schema_Storage_GET> schema_Storage_GET("storage");
		schema_Storage_GET.addToHandler( m_impl, "GET");

		static const DefineSchema<Schema_QueryAnalyzer_GET> schema_QueryAnalyzer_GET("qryanalyzer");
		schema_QueryAnalyzer_GET.addToHandler( m_impl, "GET");

		static const DefineSchema<Schema_QueryEval_GET> schema_QueryEval_GET("qryeval");
		schema_QueryEval_GET.addToHandler( m_impl, "GET");

		// [2] Add methods
		static const IntrospectionMethodDescription mt_Context_GET( mt::Context::introspection(), "config");
		mt_Context_GET.addToHandler( m_impl);

		static const IntrospectionMethodDescription mt_DocumentAnalyzer_GET( mt::DocumentAnalyzer::introspection(), "docanalyzer");
		mt_DocumentAnalyzer_GET.addToHandler( m_impl);

		static const TransformationMethodDescription mt_DocumentAnalyzer_GET_content( mt::DocumentAnalyzer::analyzeMultiPart(), "doc");
		mt_DocumentAnalyzer_GET_content.addToHandler( m_impl);

		static const IntrospectionMethodDescription mt_QueryAnalyzer_GET( mt::QueryAnalyzer::introspection(), "qryanalyzer");
		mt_QueryAnalyzer_GET.addToHandler( m_impl);

		static const IntrospectionMethodDescription mt_QueryEval_GET( mt::QueryEval::introspection(), "qryeval");
		mt_QueryEval_GET.addToHandler( m_impl);

		static const IntrospectionMethodDescription mt_StatisticsServer_GET( mt::StatisticsMap::introspection(), "statserver");
		mt_StatisticsServer_GET.addToHandler( m_impl);

		static const IntrospectionMethodDescription mt_StorageClient_GET( mt::StorageClient::introspection(), "storage");
		mt_StorageClient_GET.addToHandler( m_impl);
		static const PostTransactionMethodDescription mt_StorageClient_POST_transaction( mt::StorageClient::createTransaction(), "transaction");
		mt_StorageClient_POST_transaction.addToHandler( m_impl);
		static const CommitTransactionMethodDescription mt_StorageTransaction_COMMIT( mt::StorageTransaction::commit());
		mt_StorageTransaction_COMMIT.addToHandler( m_impl);

		static const IntrospectionMethodDescription mt_VectorStorageClient_GET( mt::VectorStorageClient::introspection(), "vstorage");
		mt_VectorStorageClient_GET.addToHandler( m_impl);
		static const PostTransactionMethodDescription mt_VectorStorageClient_POST_transaction( mt::VectorStorageClient::createTransaction(), "transaction");
		mt_VectorStorageClient_POST_transaction.addToHandler( m_impl);
		static const CommitTransactionMethodDescription mt_VectorStorageTransaction_COMMIT( mt::VectorStorageTransaction::commit());
		mt_VectorStorageTransaction_COMMIT.addToHandler( m_impl);

		static const IntrospectionMethodDescription mt_Inserter_GET( mt::Inserter::introspection(), "inserter");
		mt_Inserter_GET.addToHandler( m_impl);
		static const PostTransactionMethodDescription mt_Inserter_POST_transaction( mt::Inserter::createTransaction(), "transaction");
		mt_Inserter_POST_transaction.addToHandler( m_impl);
		static const InsertMethodDescription mt_InserterTransaction_PUT( mt::InserterTransaction::insertDocument());
		mt_InserterTransaction_PUT.addToHandler( m_impl);
		static const CommitTransactionMethodDescription mt_InserterTransaction_COMMIT( mt::InserterTransaction::commit());
		mt_InserterTransaction_COMMIT.addToHandler( m_impl);

		static const IntrospectionMethodDescription mt_ContentStatistics_GET( mt::ContentStatistics::introspection(), "contentstats");
		mt_ContentStatistics_GET.addToHandler( m_impl);
		static const PostTransactionMethodDescription mt_ContentStatistics_POST_transaction( mt::ContentStatistics::createCollector(), "transaction");
		mt_ContentStatistics_POST_transaction.addToHandler( m_impl);
		static const InsertMethodDescription mt_ContentStatisticsTransaction_PUT( mt::ContentStatisticsCollector::putContent());
		mt_ContentStatisticsTransaction_PUT.addToHandler( m_impl);
		static const DumpMethodDescription mt_ContentStatisticsTransaction_GET( mt::ContentStatisticsCollector::statistics(), "statistics");
		mt_ContentStatisticsTransaction_GET.addToHandler( m_impl);

		loadConfiguration( configstr_);
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

void WebRequestHandler::init()
{
	m_configHandler.clearUnfinishedTransactions();
	m_configHandler.deleteObsoleteConfigurations();
}

static std::string getSchemaFileName( const std::string& dir, const std::string& doctype, const std::string& schematype, const std::string& schemaname)
{
	std::string filename( strus::joinFilePath( dir, doctype));
	if (schematype == "" && schemaname == ROOT_CONTEXT_NAME)
	{
		filename = strus::joinFilePath( filename, "config");
	}
	else if (schematype == ROOT_CONTEXT_NAME)
	{
		filename = strus::joinFilePath( filename, schemaname);
	}
	else
	{
		filename = strus::joinFilePath( strus::joinFilePath( filename, schemaname), schematype);
	}
	return filename;
}

static papuga_ContentType getSchemaContentType( const std::string& doctype)
{
	if (strus::caseInsensitiveEquals( doctype, "xml") || strus::caseInsensitiveEquals( doctype, "xsd"))
	{
		return papuga_ContentType_XML;
	}
	else if (strus::caseInsensitiveEquals( doctype, "json"))
	{
		return papuga_ContentType_JSON;
	}
	else
	{
		throw strus::runtime_error( _TXT("unknown content type to print schemas: '%s'"), doctype.c_str());
	}
}

static std::string getSchemaExtension( const std::string& doctype)
{
	if (strus::caseInsensitiveEquals( doctype, "xml") || strus::caseInsensitiveEquals( doctype, "xsd"))
	{
		return ".xsd";
	}
	else if (strus::caseInsensitiveEquals( doctype, "json"))
	{
		return ".jsd";
	}
	else
	{
		throw strus::runtime_error( _TXT("unknown content type to print schemas: '%s'"), doctype.c_str());
	}
}

void WebRequestHandler::storeSchemaDescriptions( const std::string& dir, const std::string& doctype) const
{
	enum {BufSize=1024};
	char const* typebuf[ BufSize];
	char const* namebuf[ BufSize];
	int ec;
	papuga_ContentType contentType = getSchemaContentType( doctype);
	std::string extension = getSchemaExtension( doctype);

	char const** types = papuga_RequestHandler_list_schema_types( m_impl, typebuf, BufSize);
	const char* const* ti = types;
	for (; *ti; ++ti)
	{
		char const** names = papuga_RequestHandler_list_schema_names( m_impl, *ti, namebuf, BufSize);
		const char* const* ni = names;
		for (; *ni; ++ni)
		{
			std::string schemaFileName( getSchemaFileName( dir, doctype, *ti/*schematype*/, *ni/*schemaname*/));
			std::string schemaDirName;
			ec = strus::getParentPath( schemaFileName, schemaDirName);
			if (ec) throw strus::runtime_error( _TXT("error evaluating directory for schema descriptions for file '%s': %s"), schemaFileName.c_str(), ::strerror(ec));
			if (!schemaDirName.empty())
			{
				ec = strus::mkdirp( schemaDirName);
				if (ec) throw strus::runtime_error( _TXT("error creating directory for schema descriptions '%s': %s"), schemaDirName.c_str(), ::strerror(ec));
			}
			const papuga_SchemaDescription* descr = papuga_RequestHandler_get_description( m_impl, *ti/*schematype*/, *ni/*schemaname*/);
			papuga_Allocator allocator;
			papuga_init_Allocator( &allocator, NULL, 0);
			std::size_t textlen;
			const char* text = (const char*)papuga_SchemaDescription_get_text( descr, &allocator, contentType, papuga_UTF8, &textlen);
			if (!text)
			{
				papuga_destroy_Allocator( &allocator);
				throw strus::runtime_error( _TXT("error creating schema descriptions '%s': %s"), schemaFileName.c_str(), papuga_ErrorCode_tostring( papuga_SchemaDescription_last_error( descr)));
			}
			try
			{
				std::string textstr( text, textlen);
				ec = strus::writeFile( schemaFileName + extension, textstr);
				papuga_destroy_Allocator( &allocator);
				if (ec) throw strus::runtime_error( _TXT("error writing file for schema description '%s': %s"), schemaFileName.c_str(), ::strerror(ec));
			}
			catch (const std::bad_alloc&)
			{
				papuga_destroy_Allocator( &allocator);
				throw std::bad_alloc();
			}
		}
	}
}

void WebRequestHandler::storeSchemaDescriptions( const std::string& dir) const
{
	storeSchemaDescriptions( dir, "xml");
	storeSchemaDescriptions( dir, "json");
}

void WebRequestHandler::clear()
{
	m_eventLoop->stop();
	if (m_impl) {papuga_destroy_RequestHandler( m_impl); m_impl=0;}
}

WebRequestHandler::~WebRequestHandler()
{
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

bool WebRequestHandler::delegateRequest(
		const std::string& address,
		const std::string& method,
		const std::string& content,
		WebRequestDelegateContextInterface* context)
{
	/*[-]*/std::cerr << "+++ send " << method << " " << address << " {" << content << "}" << std::endl;
	if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogDelegateRequests))
	{
		m_logger->logDelegateRequest( address.c_str(), method.c_str(), content.c_str());
	}
	return m_eventLoop->send( address, method, content, context);
}

void WebRequestHandler::tick()
{
	m_transactionPool.collectGarbage( m_eventLoop->time());
}

void WebRequestHandler::loadSubConfiguration( WebRequestContextInterface* ctxi, const ConfigurationDescription& cfg)
{
	WebRequestAnswer status;

	std::vector<WebRequestDelegateRequest> delegates;
	strus::WebRequestContent subcontent( g_config_charset, cfg.doctype.c_str(), cfg.contentbuf.c_str(), cfg.contentbuf.size());
	if (!ctxi->executeLoadSubConfiguration( cfg.type.c_str(), cfg.name.c_str(), subcontent, status, delegates))
	{
		throw strus::runtime_error(
			_TXT("error loading sub configuration %s '%s': %s"),
			cfg.type.c_str(), cfg.name.c_str(), status.errorstr());
	}
	std::vector<WebRequestDelegateRequest>::const_iterator di = delegates.begin(), de = delegates.end();
	for (; di != de; ++di)
	{
		std::string contentstr( di->contentstr(), di->contentlen());
		m_eventLoop->send(
			di->url(), di->method(), contentstr,
			new ConfigurationUpdateRequestContext( this, m_logger, cfg.type, cfg.name, di->receiverSchema()));
	}
	m_configHandler.declareSubConfiguration( cfg.type.c_str(), cfg.name.c_str());
}

void WebRequestHandler::loadConfiguration( const std::string& configstr)
{
	WebRequestAnswer status;

	if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogConfiguration))
	{
		m_logger->logPutConfiguration( ROOT_CONTEXT_NAME, ROOT_CONTEXT_NAME, configstr);
	}
	strus::local_ptr<WebRequestContextInterface> ctx( createContext( g_config_accepted_charset, g_config_accepted_doctype, ""/*html_base_href*/, status));
	WebRequestContextInterface* ctxi = ctx.get();
	if (!ctxi) throw std::runtime_error( status.errorstr() ? status.errorstr() : _TXT("unknown error"));

	// Load main configuration:
	strus::WebRequestContent content( g_config_charset, g_config_doctype, configstr.c_str(), configstr.size());
	if (!ctxi->executeLoadMainConfiguration( content, status))
	{
		throw std::runtime_error( status.errorstr() ? status.errorstr() : _TXT("unknown error"));
	}
	// Load sub configurations:
	std::vector<ConfigurationDescription> cfglist = m_configHandler.getSubConfigurations( configstr);
	std::vector<ConfigurationDescription>::const_iterator ci = cfglist.begin(), ce = cfglist.end();
	for (; ci != ce; ++ci)
	{
		loadSubConfiguration( ctxi, *ci);
	}
	// Load put configurations:
	cfglist = m_configHandler.getStoredConfigurations();
	ci = cfglist.begin(), ce = cfglist.end();
	for (; ci != ce; ++ci)
	{
		loadSubConfiguration( ctxi, *ci);
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



