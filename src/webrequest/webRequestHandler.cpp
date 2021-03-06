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
#include "strus/base/sleep.hpp"
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
		:MethodDescription( "GET", id, 200, NULL, rootelem, 0/*no list*/, false/*has content*/, 1, WebRequestHandler::ParamPathArray){}
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
		:MethodDescription( "PUT", id, 204/*no content*/, NULL, "insert", 0/*no list*/, true/*has content*/, 2, WebRequestHandler::ParamPathString, WebRequestHandler::ParamContent){}
};
struct CommitTransactionMethodDescription
	:public MethodDescription
{
	CommitTransactionMethodDescription( const papuga_RequestMethodId& id)
		:MethodDescription( "PUT/transaction", id, 204/*no content*/, NULL, "commit", 0/*no list*/, false/*has content*/, 0){}
};
struct PostTransactionMethodDescription
	:public MethodDescription
{
	PostTransactionMethodDescription( const papuga_RequestMethodId& id, const char* rootelem)
		:MethodDescription( "POST/transaction", id, 200, NULL, rootelem, 0/*no list*/, false/*has content*/, 0){}
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

template <class SCHEMA>
class DefineUpdateConfigSchema
	:public DefineSchema<SCHEMA>
{
public:
	DefineUpdateConfigSchema( const char* contextType) :DefineSchema<SCHEMA>( contextType){}
};

static const char* g_context_typenames[] = {"contentstats","statserver","storage","vstorage","docanalyzer","qryanalyzer","qryeval","distqryeval","inserter",0};

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
		int port_,
		bool beautifiedOutput_,
		int maxIdleTime_,
		int nofTransactionsPerSeconds)
	:m_debug_maxdepth(logger_?logger_->structDepth():0)
	,m_logger(logger_)
	,m_impl(0)
	,m_configHandler(logger_,config_store_dir_,service_name_,g_context_typenames)
	,m_html_head(html_head_)
	,m_serviceName(service_name_)
	,m_transactionPool( eventLoop_->time(), maxIdleTime_*2, nofTransactionsPerSeconds, logger_)
	,m_port((port_==80||port_==0) ? std::string() : strus::string_format("%d",port_))
	,m_maxIdleTime(maxIdleTime_)
	,m_beautifiedOutput(beautifiedOutput_)
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

		static const DefineConfigSchema<Schema_Context_POST_Storage> schema_Context_POST_Storage;
		schema_Context_POST_Storage.addToHandler( m_impl, "POST/storage");
		static const DefineConfigSchema<Schema_Context_DELETE_POST_Storage> schema_Context_DELETE_POST_Storage;
		schema_Context_DELETE_POST_Storage.addToHandler( m_impl, "DELETE_POST/storage");
		static const DefineConfigSchema<Schema_Context_PUT_Storage> schema_Context_PUT_Storage;
		schema_Context_PUT_Storage.addToHandler( m_impl, "PUT/storage");
		static const DefineUpdateConfigSchema<Schema_Storage_PUT> schema_Storage_PUT( "storage");
		schema_Storage_PUT.addToHandler( m_impl, "PUT");
		static const DefineSchema<Schema_StorageTransaction_PUT> schema_StorageTransaction_PUT("transaction/storage");
		schema_StorageTransaction_PUT.addToHandler( m_impl, "PUT");

		static const DefineConfigSchema<Schema_Context_POST_VectorStorage> schema_Context_POST_VectorStorage;
		schema_Context_POST_VectorStorage.addToHandler( m_impl, "POST/vstorage");
		static const DefineConfigSchema<Schema_Context_DELETE_POST_VectorStorage> schema_Context_DELETE_POST_VectorStorage;
		schema_Context_DELETE_POST_VectorStorage.addToHandler( m_impl, "DELETE_POST/vstorage");
		static const DefineConfigSchema<Schema_Context_PUT_VectorStorage> schema_Context_PUT_VectorStorage;
		schema_Context_PUT_VectorStorage.addToHandler( m_impl, "PUT/vstorage");
		static const DefineSchema<Schema_Context_PUT_VectorStorageTransaction> schema_Context_PUT_VectorStorageTransaction("transaction/vstorage");
		schema_Context_PUT_VectorStorageTransaction.addToHandler( m_impl, "PUT");

		static const DefineConfigSchema<Schema_Context_PUT_DocumentAnalyzer> schema_Context_POST_DocumentAnalyzer;
		schema_Context_POST_DocumentAnalyzer.addToHandler( m_impl, "POST/docanalyzer");
		static const DefineConfigSchema<Schema_Context_PUT_DocumentAnalyzer> schema_Context_PUT_DocumentAnalyzer;
		schema_Context_PUT_DocumentAnalyzer.addToHandler( m_impl, "PUT/docanalyzer");

		static const DefineConfigSchema<Schema_Context_POST_QueryAnalyzer> schema_Context_POST_QueryAnalyzer;
		schema_Context_POST_QueryAnalyzer.addToHandler( m_impl, "POST/qryanalyzer");
		static const DefineConfigSchema<Schema_Context_PUT_QueryAnalyzer> schema_Context_PUT_QueryAnalyzer;
		schema_Context_PUT_QueryAnalyzer.addToHandler( m_impl, "PUT/qryanalyzer");

		static const DefineConfigSchema<Schema_Context_POST_QueryEval> schema_Context_POST_QueryEval;
		schema_Context_POST_QueryEval.addToHandler( m_impl, "POST/qryeval");
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

		static const DefineConfigSchema<Schema_Context_POST_DistQueryEval> schema_Context_POST_DistQueryEval;
		schema_Context_POST_DistQueryEval.addToHandler( m_impl, "POST/distqryeval");
		static const DefineConfigSchema<Schema_Context_PUT_DistQueryEval> schema_Context_PUT_DistQueryEval;
		schema_Context_PUT_DistQueryEval.addToHandler( m_impl, "PUT/distqryeval");

		static const DefineSchema<Schema_DistQueryEval_GET> schema_DistQueryEval_GET("distqryeval");
		schema_DistQueryEval_GET.addToHandler( m_impl, "GET");
		static const DefineSchema<Schema_DistQueryEval_SET_analysis> schema_DistQueryEval_SET_analysis("distqryeval");
		schema_DistQueryEval_SET_analysis.addToHandler( m_impl, "SET~analysis");
		static const DefineSchema<Schema_DistQueryEval_SET_querystats> schema_DistQueryEval_SET_querystats("distqryeval");
		schema_DistQueryEval_SET_querystats.addToHandler( m_impl, "SET~querystats");
		static const DefineSchema<Schema_DistQueryEval_SET_collect> schema_DistQueryEval_SET_collect("distqryeval");
		schema_DistQueryEval_SET_collect.addToHandler( m_impl, "SET~collect");
		static const DefineSchema<Schema_DistQueryEval_CLOSE_collect> schema_DistQueryEval_CLOSE_collect("distqryeval");
		schema_DistQueryEval_CLOSE_collect.addToHandler( m_impl, "CLOSE~collect");
		static const DefineSchema<Schema_DistQueryEval_SET_ranklist> schema_DistQueryEval_SET_ranklist("distqryeval");
		schema_DistQueryEval_SET_ranklist.addToHandler( m_impl, "SET~ranklist");
		static const DefineSchema<Schema_DistQueryEval_END_ranklist> schema_DistQueryEval_END_ranklist("distqryeval");
		schema_DistQueryEval_END_ranklist.addToHandler( m_impl, "END~ranklist");

		static const DefineSchema<Schema_Storage_GET> schema_Storage_GET("storage");
		schema_Storage_GET.addToHandler( m_impl, "GET");
		static const DefineSchema<Schema_StatisticsServer_GET> schema_StatisticsServer_GET("statserver");
		schema_StatisticsServer_GET.addToHandler( m_impl, "GET");
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
	}
	catch (const std::bad_alloc&)
	{
		papuga_destroy_RequestHandler( m_impl);
		throw std::bad_alloc();
	}
	catch (const std::runtime_error& err)
	{
		papuga_destroy_RequestHandler( m_impl);
		throw err;
	}
}

WebRequestHandler::~WebRequestHandler()
{
	papuga_destroy_RequestHandler( m_impl);
}

static void setAnswer( WebRequestAnswer& answer, ErrorCode errcode, const char* errstr=0, bool doCopy=false)
{
	int httpstatus = errorCodeToHttpStatus( errcode);
	if (errstr)
	{
		answer.setError( httpstatus, errcode, errstr, doCopy);
	}
	else
	{
		answer.setError( httpstatus, errcode, strus::errorCodeToString(errcode));
	}
}

#define WEBREQUEST_HANDLER_CATCH_ERROR_RETURN( answer, errorReturnValue) \
	catch (const std::bad_alloc&) \
	{\
		setAnswer( answer, ErrorCodeOutOfMem);\
		return errorReturnValue;\
	}\
	catch (const std::runtime_error& err)\
	{\
		setAnswer( answer, ErrorCodeRuntimeError, err.what(), true/*do copy*/);\
		return errorReturnValue;\
	}\
	catch (...)\
	{\
		setAnswer( answer, ErrorCodeUncaughtException);\
		return errorReturnValue;\
	}

bool WebRequestHandler::init( 
		const std::string& configsrc,
		WebRequestAnswer& answer)
{
	try
	{
		m_configHandler.clearUnfinishedTransactions();
		m_configHandler.deleteObsoleteConfigurations();

		return loadConfiguration( configsrc, answer);
	}
	WEBREQUEST_HANDLER_CATCH_ERROR_RETURN( answer, false);
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

WebRequestContextInterface* WebRequestHandler::createConfigurationContext(
		const char* contextType,
		const char* contextName,
		WebRequestAnswer& answer)
{
	try
	{
		return new WebRequestContext( this, m_logger, &m_configHandler, &m_transactionPool, contextType, contextName);
	}
	WEBREQUEST_HANDLER_CATCH_ERROR_RETURN( answer, NULL);
}

WebRequestContextInterface* WebRequestHandler::createContext(
		const char* accepted_charset,
		const char* accepted_doctype,
		const char* html_base_href,
		const char* method,
		const char* path,
		WebRequestAnswer& answer)
{
	try
	{
		return new WebRequestContext( this, m_logger, &m_configHandler, &m_transactionPool, accepted_charset, accepted_doctype, html_base_href, method, path);
	}
	WEBREQUEST_HANDLER_CATCH_ERROR_RETURN( answer, NULL);
}

bool WebRequestHandler::delegateRequest(
		const std::string& address,
		const std::string& method,
		const std::string& content,
		WebRequestDelegateContextInterface* context)
{
	if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogDelegateRequests))
	{
		m_logger->logDelegateRequest( address.c_str(), method.c_str(), content.c_str(), content.size());
	}
	return m_eventLoop->send( address, method, content, context);
}

WebRequestAnswer WebRequestHandler::getSimpleRequestAnswer(
		const char* accepted_charset,
		const char* accepted_doctype,
		const char* html_base_href,
		const std::string& name,
		const std::string& message)
{
	WebRequestAnswer rt;
	try
	{
		papuga_StringEncoding result_encoding = strus::getResultStringEncoding( accepted_charset, WebRequestContext::defaultEncoding());
		WebRequestContent::Type result_doctype = strus::getResultContentType( accepted_doctype, WebRequestContext::defaultDocType());
		if (result_encoding == papuga_Binary)
		{
			rt.setError_fmt( strus::errorCodeToHttpStatus( ErrorCodeNotImplemented), ErrorCodeNotImplemented, _TXT("none of the accept charsets implemented: %s"), accepted_charset);
			return rt;
		}
		if (result_doctype == WebRequestContent::Unknown)
		{
			rt.setError_fmt( strus::errorCodeToHttpStatus( ErrorCodeNotImplemented), ErrorCodeNotImplemented, _TXT("none of the accept content types implemented: %s"), accepted_doctype);
			return rt;
		}
		(void)strus::mapStringToAnswer( rt, 0/*allocator*/, html_head(), ""/*html href base*/, SYSTEM_MESSAGE_HEADER, name.c_str(), result_encoding, result_doctype, m_beautifiedOutput, message);
	}
	catch (const std::bad_alloc&)
	{
		setAnswer( rt, ErrorCodeOutOfMem);
	}
	catch (const std::runtime_error& err)
	{
		setAnswer( rt, ErrorCodeRuntimeError, err.what(), true);
	}
	return rt;
}

void WebRequestHandler::tick()
{
	m_transactionPool.collectGarbage( m_eventLoop->time());
}

static const char* parsePathDelim( char const* ai)
{
	if (*ai == '/')
	{
		for (++ai; *ai == '/'; ++ai){}
		return ai;
	}
	else if (*ai == '\0')
	{
		return ai;
	}
	return NULL;
}

static const char* parseLocalHost( char const* ai)
{
	if (ai)
	{
		if (0==std::memcmp( ai, "http:/", 6))
		{
			if (!(ai = parsePathDelim( ai+6))) return NULL;
		}
		else if (0==std::memcmp( ai, "https:/", 7))
		{
			if (!(ai = parsePathDelim( ai+7))) return NULL;
		}
		if (0==std::memcmp( ai, "127.0.0.1", 9))
		{
			return ai+9;
		}
		else if (0==std::memcmp( ai, "localhost", 9))
		{
			return ai+9;
		}
	}
	return NULL;
}

static const char* parsePort( const char* ai, const std::string& port)
{
	if (ai)
	{
		if (*ai == '/' || *ai == '\0')
		{
			if (port.empty()) return parsePathDelim( ai);
		}
		else if (*ai == ':')
		{
			++ai;
			if (port.empty())
			{
				if (ai[0] == '8' && ai[1] == '0') return parsePathDelim( ai+2);
			}
			else if (0==std::memcmp(ai,port.c_str(),port.size()))
			{
				return parsePathDelim( ai+port.size());
			}
		}
	}
	return NULL;
}

const char* WebRequestHandler::pathToSelf( const char* address)
{
	return parsePort( parseLocalHost( address), m_port);
}

bool WebRequestHandler::loopbackConfigurationLoadDelegateRequest( WebRequestContextInterface* receiverContext, const char* receiverSchema, const char* method, const char* path, const std::string& contentstr, WebRequestAnswer& answer)
{
	WebRequestContent content( g_config_charset, g_config_doctype, contentstr.c_str(), contentstr.size());
	strus::Reference<WebRequestContextInterface> ctx( createContext( 
			"UTF-8"/*accepted_charset*/, "application/json"/*accepted_doctype*/,""/*html_base_href*/,
			method, path, answer));
	if (ctx.get())
	{
		WebRequestAnswer delegateAnswer;
		if (!runConfigurationLoad( ctx.get(), content, delegateAnswer))
		{
			answer = delegateAnswer;
			answer.explain( _TXT("configuration loopback delegate request failed"));
			return false;
		}
		if (receiverSchema)
		{
			if (!receiverContext->putDelegateRequestAnswer( receiverSchema, delegateAnswer))
			{
				answer = receiverContext->getAnswer();
				answer.explain( _TXT("put of configuration loopback delegate request answer failed"));
				return false;
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

bool WebRequestHandler::runConfigurationLoad( WebRequestContextInterface* ctx, const WebRequestContent& content, WebRequestAnswer& answer)
{
	bool rt = true;
	if (!ctx->execute( content))
	{
		answer = ctx->getAnswer();
		return false;
	}
	std::vector<WebRequestDelegateRequest> delegates = ctx->getDelegateRequests();
	while (rt && !delegates.empty())
	{
		strus::AtomicCounter<int> count( delegates.size());
		std::vector<WebRequestDelegateRequest>::const_iterator di = delegates.begin(), de = delegates.end();
		for (int didx=0; di != de; ++di,++didx)
		{
			try
			{
				std::string delegate_contentstr( di->contentstr(), di->contentlen());
				if (di->url())
				{
					const char* pt = pathToSelf( di->url());
					if (pt)
					{
						if (!loopbackConfigurationLoadDelegateRequest( ctx, di->receiverSchema(), di->method(), pt, delegate_contentstr, answer))
						{
							count.set( 0);
							rt = false;
							break;
						}
						count.decrement();
					}
					else
					{
						ConfigurationUpdateRequestContext* update = new ConfigurationUpdateRequestContext( this, m_logger, ctx, di->receiverSchema(), &count);
						if (!m_eventLoop->send( di->url(), di->method(), delegate_contentstr, update))
						{
							count.set( 0);
							rt = false;
							break;
						}
					}
				}
				else
				{
					// ... without receiver feed to itself with the receiver schema:
					WebRequestContent delegate_content( g_config_charset, g_config_doctype, delegate_contentstr.c_str(), delegate_contentstr.size());
					WebRequestAnswer delegate_answer( delegate_content);
					if (di->receiverSchema())
					{
						if (!ctx->putDelegateRequestAnswer( di->receiverSchema(), delegate_answer))
						{
							count.set( 0);
							rt = false;
							break;
						}
					}
					count.decrement();
				}
			}
			catch (const std::bad_alloc&)
			{
				setAnswer( answer, ErrorCodeOutOfMem);
				count.set( 0);
				rt = false;
				break;
			}
		}
		while (count.value() > 0)
		{
			strus::usleep( 50);
			// ... we poll with a sleep of 20ms as it is not time critical to do this in the initialization phase
		}
		if (rt) delegates = ctx->getDelegateRequests();
	}
	if (rt) rt = ctx->complete();
	answer = ctx->getAnswer();
	return rt;
}

bool WebRequestHandler::loadSubConfiguration( const ConfigurationDescription& cfg, bool initload, WebRequestAnswer& answer)
{
	m_configHandler.declareSubConfiguration( cfg.type, cfg.name);
	strus::local_ptr<WebRequestContextInterface> ctx( createConfigurationContext( cfg.type.c_str(), cfg.name.c_str(), answer));
	WebRequestContextInterface* ctxi = ctx.get();
	if (!ctxi)
	{
		answer.explain( initload
			? _TXT("error creating context for loading embedded sub configuration")
			: _TXT("error creating context for restoring context"));
		return false;
	}
	const char* doctype = WebRequestContent::typeName(cfg.doctype);
	WebRequestContent subcontent( g_config_charset, doctype, cfg.contentbuf.c_str(), cfg.contentbuf.size());
	if (!runConfigurationLoad( ctxi, subcontent, answer))
	{
		answer.explain( initload
			? _TXT("error loading embedded sub configuration")
			: _TXT("error restoring context"));
		return false;
	}
	if (!answer.content().empty())
	{
		setAnswer( answer, ErrorCodeLogicError, _TXT("load configuration request returned content"));
		return false;
	}
	return true;
}

bool WebRequestHandler::loadMainConfiguration( const std::string& configstr, WebRequestAnswer& answer)
{
	if (!!(m_logger->logMask() & WebRequestLoggerInterface::LogConfiguration))
	{
		m_logger->logPutConfiguration( ROOT_CONTEXT_NAME, ROOT_CONTEXT_NAME, configstr);
	}
	strus::local_ptr<WebRequestContextInterface> ctx( createConfigurationContext( 0, 0, answer));
	WebRequestContextInterface* ctxi = ctx.get();
	if (!ctxi)
	{
		answer.explain( _TXT("error creating context for loading main configuration"));
		return false;
	}
	strus::WebRequestContent content( g_config_charset, g_config_doctype, configstr.c_str(), configstr.size());
	if (!runConfigurationLoad( ctxi, content, answer))
	{
		answer.explain( _TXT("error loading main configuration"));
		return false;
	}
	if (!answer.content().empty())
	{
		setAnswer( answer, ErrorCodeLogicError, _TXT("load configuration request returned content"));
		return false;
	}
	return true;
}

bool WebRequestHandler::loadConfiguration( const std::string& configstr, WebRequestAnswer& answer)
{
	// Load main configuration:
	if (!loadMainConfiguration( configstr, answer)) return false;

	// Load embedded sub configurations:
	std::vector<ConfigurationDescription> cfglist = m_configHandler.getSubConfigurations( configstr);
	std::vector<ConfigurationDescription>::const_iterator ci = cfglist.begin(), ce = cfglist.end();
	for (; ci != ce; ++ci)
	{
		if (!loadSubConfiguration( *ci, true, answer)) return false;
	}
	// Load put configurations:
	cfglist = m_configHandler.getStoredConfigurations();
	ci = cfglist.begin(), ce = cfglist.end();
	for (; ci != ce; ++ci)
	{
		if (!loadSubConfiguration( *ci, false, answer)) return false;
	}
	return true;
}

bool WebRequestHandler::transferContext(
		const char* contextType,
		const char* contextName,
		papuga_RequestContext* context,
		WebRequestAnswer& answer)
{
	papuga_ErrorCode errcode = papuga_Ok;
	strus::unique_lock lock( m_mutex_context_transfer);
	if (!papuga_RequestHandler_transfer_context( m_impl, contextType, contextName, context, &errcode))
	{
		papuga_destroy_RequestContext( context);
		setAnswer( answer, papugaErrorToErrorCode( errcode));
		return false;
	}
	return true;
}

bool WebRequestHandler::transferConfigurationContext(
		const ConfigurationTransaction& configTransaction,
		papuga_RequestContext* context,
		WebRequestAnswer& answer)
{
	papuga_ErrorCode errcode = papuga_Ok;
	strus::unique_lock lock( m_mutex_context_transfer);
	if (!papuga_RequestHandler_transfer_context( m_impl, configTransaction.type.c_str(), configTransaction.name.c_str(), context, &errcode))
	{
		papuga_destroy_RequestContext( context);
		setAnswer( answer, papugaErrorToErrorCode( errcode));
		return false;
	}
	const char* errmsg = 0;
	char errbuf[ 2048];
	try
	{
		m_configHandler.commitStoreConfiguration( configTransaction);
		return true;
	}
	catch (const std::bad_alloc&)
	{
		errmsg = _TXT("fatal: out of memory in a bad moment (commit of the configuration after transfer of the context). The state of the server is now not consistent anymore with the state stored. Retry the operation to archieve consistency.");
	}
	catch (const std::runtime_error& err)
	{
		std::snprintf( errbuf, sizeof(errbuf), _TXT("fatal: file rename operation failed: %s, commit of the configuration after transfer of the context failed. The state of the server is now not consistent anymore with the state stored. Retry the operation to archieve consistency."), err.what());
		errbuf[ sizeof(errbuf)-1] = 0;
		errmsg = errbuf;
		return false;
	}
	m_logger->logError( errmsg);
	setAnswer( answer, ErrorCodeIOError, _TXT("server in inconsistent state, see logs"));
	return false;
}

bool WebRequestHandler::removeContext(
		const char* contextType,
		const char* contextName,
		WebRequestAnswer& answer)
{
	papuga_ErrorCode errcode = papuga_Ok;
	strus::unique_lock lock( m_mutex_context_transfer);
	if (!papuga_RequestHandler_remove_context( m_impl, contextType, contextName, &errcode))
	{
		setAnswer( answer, papugaErrorToErrorCode( errcode));
		return false;
	}
	return true;
}


