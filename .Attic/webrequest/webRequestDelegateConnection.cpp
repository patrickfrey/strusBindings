/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Implementation of the context of a delegated request (sub request to another server)
/// \file "webRequestDelegateConnection.cpp"
#include "webRequestDelegateConnection.hpp"
#include "strus/reference.hpp"
#include "strus/webRequestContent.hpp"
#include "strus/webRequestDelegateContextInterface.hpp"
#include "strus/base/string_format.hpp"
#include "private/internationalization.hpp"
#include "private/errorUtils.hpp"

using namespace strus;

template <typename DATA>
static void set_curl_opt( CURL *curl, CURLoption opt, const DATA& data)
{
	CURLcode res = curl_easy_setopt( curl, opt, data);
	if (res != CURLE_OK)
	{
		throw strus::runtime_error( "failed set socket option: %s", curl_easy_strerror(res));
	}
}

static void set_http_header( struct curl_slist*& headers, const char* name, const std::string& value)
{
	char buf[1024];
	if ((int)sizeof(buf) < std::snprintf( buf, sizeof(buf), "%s: %s", name, value.c_str())) throw std::bad_alloc();
	struct curl_slist* new_headers = curl_slist_append( headers, buf);
	if (!new_headers) throw std::bad_alloc();
	headers = new_headers;
}

static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	size_t nn = size*nmemb;
	std::string* output = (std::string*)userdata;
	try
	{
		output->append( (char*)ptr, nn);
	}
	catch (...)
	{
		return 0;
	}
	return nn;
}

struct WebRequestDelegateConnectionGlobals
{
	std::string user_agent;
	struct curl_slist* headers;

	WebRequestDelegateConnectionGlobals()
		:user_agent(strus::string_format( "libcurl/%s",curl_version_info(CURLVERSION_NOW)->version)),headers(0)
	{
		curl_global_init( CURL_GLOBAL_ALL);
	}
};
static WebRequestDelegateConnectionGlobals g_delegateRequestGlobals;

static void parseAddress( const std::string& address, std::string& url, int& port)
{
	url.clear();

	char const* pi = address.c_str() + address.size();
	while (pi > address.c_str() && *(pi-1) >= '0' && *(pi-1) <= '9') {--pi;}
	if (pi > address.c_str() && *(pi-1) == ':')
	{
		port = atoi( pi);
		--pi;
		url.append( address.c_str(), pi - address.c_str());
	}
	else
	{
		url = address;
	}
}

void WebRequestDelegateConnection::connect()
{
	set_curl_opt( m_curl, CURLOPT_USERAGENT, g_delegateRequestGlobals.user_agent.c_str());
	set_curl_opt( m_curl, CURLOPT_POST, 1);
	set_curl_opt( m_curl, CURLOPT_HTTPHEADER, g_delegateRequestGlobals.headers);
	set_curl_opt( m_curl, CURLOPT_FAILONERROR, 0);
	set_curl_opt( m_curl, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_1_1);
	set_curl_opt( m_curl, CURLOPT_URL, m_url.c_str());
	set_curl_opt( m_curl, CURLOPT_PORT, m_port);
	set_curl_opt( m_curl, CURLOPT_TCP_KEEPALIVE, 1L);
	set_curl_opt( m_curl, CURLOPT_TCP_KEEPIDLE, 120L);
	set_curl_opt( m_curl, CURLOPT_TCP_KEEPINTVL, 60L);
	set_curl_opt( m_curl, CURLOPT_CONNECT_ONLY, 1L);
	CURLcode res = curl_easy_perform( m_curl);
	if (res != CURLE_OK)
	{
		throw strus::runtime_error( _TXT("failed to connect to '%s:%d': %s"), m_url.c_str(), m_port, curl_easy_strerror(res));
	}
	m_state = Connected;
}

void WebRequestDelegateConnection::sendData()
{
	if (m_requestQueue.empty()) handleConnectionException( ErrorCodeRuntimeError);

	RequestData& current = m_requestQueue.front();
	if (current.dataSent == 0)
	{
		m_response_content.clear();
		m_response_errbuf[0] = '\0';
		m_response_result = CURLE_OK;

		set_curl_opt( m_curl, CURLOPT_ERRORBUFFER, m_response_errbuf);
		set_curl_opt( m_curl, CURLOPT_CUSTOMREQUEST, current.method.c_str());
	}
	std::size_t dataSent = 0;
	const void* dataPtr = current.content.c_str() + current.dataSent;
	std::size_t dataSize = current.content.size() - current.dataSent;

	m_response_result = curl_easy_send( m_curl, dataPtr, dataSize, &dataSent);
	if (m_response_result == CURLE_AGAIN)
	{
		m_response_result = CURLE_OK;
	}
	else if (m_response_result == CURLE_OK)
	{
		current.dataSent += dataSent;
		if (current.dataSent >= current.content.size())
		{
			if (current.dataSent > current.content.size())
			{
				handleConnectionException( ErrorCodeRuntimeError);
			}
			else
			{
				m_state = ReceiveAnswer;
			}
		}
	}
	else
	{
		completeResponse();
	}
}

void WebRequestDelegateConnection::receiveData()
{
	m_state = Done;
}

void WebRequestDelegateConnection::returnAnswer( const WebRequestAnswer& answer)
{
	if (m_requestQueue.empty())
	{
		handleConnectionException( ErrorCodeRuntimeError);
	}
	else
	{
		RequestData& current = m_requestQueue.front();
		current.receiver->putAnswer( answer);
		m_requestQueue.pop();
		m_state = Connected;
	}
}

void WebRequestDelegateConnection::completeResponse()
{
	if (m_response_result == CURLE_OK)
	{
		long http_code = 0;
		curl_easy_getinfo( m_curl, CURLINFO_RESPONSE_CODE, &http_code);
		WebRequestContent answerContent( "UTF-8", "application/json", m_response_content.c_str(), m_response_content.size());
		WebRequestAnswer answer( "", http_code, 0/*apperrorcode*/, answerContent);
		returnAnswer( answer);
	}
	else
	{
		long http_code = 500;
		curl_easy_getinfo( m_curl, CURLINFO_RESPONSE_CODE, &http_code);
		WebRequestAnswer answer( m_response_errbuf, http_code, ErrorCodeDelegateRequestFailed);
		returnAnswer( answer);
	}
}

WebRequestDelegateConnection::WebRequestDelegateConnection( const std::string& address)
	:m_curl(curl_easy_init()),m_url(),m_port(80),m_response_content(),m_response_result(CURLE_OK),m_state(Init)
{
	m_response_errbuf[0] = '\0';

	if (m_curl == NULL) throw std::bad_alloc();
	parseAddress( address, m_url, m_port);

	connect();
}

void WebRequestDelegateConnection::send( const std::string& method_, const std::string& content_, const strus::shared_ptr<WebRequestDelegateContextInterface>& receiver_)
{
	strus::unique_lock lock( m_mutex);
	if (m_state == Init)
	{
		connect();
	}
	m_requestQueue.push( RequestData( method_, content_, receiver_));
	if (m_state == Connected)
	{
		sendData();
	}
}
	

SocketHandle WebRequestDelegateConnection::socket()
{
#if LIBCURL_VERSION_NUM >= 0x072D00
	curl_socket_t sockfd = 0;
	CURLcode res = curl_easy_getinfo( m_curl, CURLINFO_ACTIVESOCKET, &sockfd);
#else
	long sockfd = 0;
	CURLcode res = curl_easy_getinfo( m_curl, CURLINFO_LASTSOCKET, &sockfd);
#endif
	if (res != CURLE_OK)
	{
		throw strus::runtime_error( _TXT("failed get socket info for %s:%d: %s"), m_url.c_str(), m_port, curl_easy_strerror(res));
	}
	return sockfd;
}

WebRequestDelegateConnection::~WebRequestDelegateConnection()
{
	if (m_curl) curl_easy_cleanup( m_curl);
}
	
struct ConnectionContext
{
	WebRequestDelegateConnectionRef conn;

	ConnectionContext( WebRequestDelegateConnectionRef conn_) :conn(conn_){}
	~ConnectionContext(){}
};

static void connectionDeleterProc( void* context)
{
	ConnectionContext* contextObj = (ConnectionContext*)context;
	if (contextObj) delete contextObj;
}

void WebRequestDelegateConnection::connectionResume( void* context_, ErrorCode status)
{
	ConnectionContext* context = (ConnectionContext*)context_;
}

void WebRequestDelegateConnection::connectionReadProc( void* context_)
{
	ConnectionContext* context = (ConnectionContext*)context_;
	strus::unique_lock lock( context->conn->m_mutex);
	if (context->conn->m_state == Connected)
	{
		context->conn->sendData();
	}
}

void WebRequestDelegateConnection::connectionWriteProc( void* context_)
{
	ConnectionContext* context = (ConnectionContext*)context_;
	strus::unique_lock lock( context->conn->m_mutex);
	if (context->conn->m_state == ReceiveAnswer)
	{
		context->conn->receiveData();
	}
}

void WebRequestDelegateConnection::connectionExceptProc( void* context_)
{
	ConnectionContext* context = (ConnectionContext*)context_;
	if (context->conn->m_state == Done)
	{
		context->conn->connectionResume( context, (ErrorCode)0);
	}
	else
	{
		context->conn->connectionResume( context, ErrorCodeRuntimeError);
	}
}

WebRequestDelegateConnectionRef WebRequestDelegateConnectionPool::getOrCreateConnection( const std::string& address)
{
	ConnectionMap::const_iterator ci = connectionMap.find( address);
	if (ci == connectionMap.end())
	{
		WebRequestDelegateConnectionRef conn( new WebRequestDelegateConnection( address));
		SocketHandle socket = conn->socket();
		connectionMap[ address] = conn;
		{
			strus::Reference<ConnectionContext> context( new ConnectionContext(conn));
			if (!jobqueue->pushListener( &WebRequestDelegateConnection::connectionReadProc, context.get(), connectionDeleterProc, socket, JobQueueWorker::FdRead))
			{
				connectionMap.erase( address);
				throw strus::runtime_error(_TXT("failed to listen to connection events of '%s'"), address.c_str());
			}
			context.release();
		}{
			strus::Reference<ConnectionContext> context( new ConnectionContext(conn));
			if (!jobqueue->pushListener( &WebRequestDelegateConnection::connectionWriteProc, context.get(), connectionDeleterProc, socket, JobQueueWorker::FdWrite))
			{
				connectionMap.erase( address);
				throw strus::runtime_error(_TXT("failed to listen to connection events of '%s'"), address.c_str());
			}
			context.release();
		}{
			strus::Reference<ConnectionContext> context( new ConnectionContext(conn));
			if (!jobqueue->pushListener( &WebRequestDelegateConnection::connectionExceptProc, context.get(), connectionDeleterProc, socket, JobQueueWorker::FdExcept))
			{
				connectionMap.erase( address);
				throw strus::runtime_error(_TXT("failed to listen to connection events of '%s'"), address.c_str());
			}
			context.release();
		}
		return conn;
	}
	else
	{
		return ci->second;
	}
}

