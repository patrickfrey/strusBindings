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

static bool set_http_header( struct curl_slist*& headers, const char* name, const std::string& value)
{
	char buf[1024];
	if ((int)sizeof(buf) < std::snprintf( buf, sizeof(buf), "%s: %s", name, value.c_str())) return false;
	struct curl_slist* new_headers = curl_slist_append( headers, buf);
	if (!new_headers) return false;
	headers = new_headers;
	return true;
}

static size_t std_string_append_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
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
	bool valid;

	WebRequestDelegateConnectionGlobals()
		:user_agent(strus::string_format( "libcurl/%s",curl_version_info(CURLVERSION_NOW)->version)),headers(0)
	{
		curl_global_init( CURL_GLOBAL_ALL);
		valid = true;

		valid &= set_http_header( headers, "Expect", "");
		valid &= set_http_header( headers, "Content-Type", "application/json; charset=utf-8");
		valid &= set_http_header( headers, "Accept", "application/json");
		valid &= set_http_header( headers, "Accept-Charset", "UTF-8");
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
	if (!g_delegateRequestGlobals.valid) throw std::runtime_error(_TXT("fatal: delegate connection globals initialization failed"));

	if (m_curl) curl_easy_cleanup( m_curl);
	m_curl = curl_easy_init();
	if (m_curl == NULL) throw std::bad_alloc();

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

WebRequestDelegateConnection::WebRequestDelegateConnection( const std::string& address, CurlEventLoop* eventloop_)
	:m_curl(NULL)
	,m_url()
	,m_port(80)
	,m_state(Init)
	,m_mutex()
	,m_requestQueue()
	,m_eventloop(eventloop_)
{
	parseAddress( address, m_url, m_port);
	connect();
}

void WebRequestDelegateConnection::push( const std::string& method_, const std::string& content_, const strus::shared_ptr<WebRequestDelegateContextInterface>& receiver_)
{
	strus::unique_lock lock( m_mutex);
	if (m_state == Init)
	{
		connect();
	}
	m_requestQueue.push( WebRequestDelegateDataRef( new WebRequestDelegateData( method_, content_, receiver_)));
}

WebRequestDelegateDataRef WebRequestDelegateConnection::fetch()
{
	strus::unique_lock lock( m_mutex);
	if (m_state == Connected && !m_requestQueue.empty())
	{
		WebRequestDelegateDataRef rt = m_requestQueue.front();
		m_requestQueue.pop();
		m_state = Process;
		return rt;
	}
	else
	{
		return WebRequestDelegateDataRef();
	}
}

void WebRequestDelegateConnection::done()
{
	strus::unique_lock lock( m_mutex);
	if (m_state == Process)
	{
		m_state = Connected;
	}
	else if (m_state == Init)
	{
		connect();
	}
}

void WebRequestDelegateConnection::dropAllPendingRequests( const char* errmsg)
{
	strus::unique_lock lock( m_mutex);
	for (; !m_requestQueue.empty(); m_requestQueue.pop())
	{
		WebRequestDelegateDataRef data = m_requestQueue.front();
		WebRequestAnswer answer( errmsg, 500, ErrorCodeDelegateRequestFailed);
		data->receiver->putAnswer( answer);
	}
	m_state = Init;
}

void WebRequestDelegateConnection::reconnect()
{
	try
	{
		strus::unique_lock lock( m_mutex);
		connect();
	}
	catch (const std::runtime_error& err)
	{
		dropAllPendingRequests( err.what());
	}
	catch (const std::bad_alloc&)
	{
		dropAllPendingRequests( _TXT("memory allocation error"));
	}
	catch (...)
	{
		dropAllPendingRequests( _TXT("unexpected exception"));
	}
}

WebRequestDelegateConnection::~WebRequestDelegateConnection()
{
	if (m_curl) curl_easy_cleanup( m_curl);
}

WebRequestDelegateJob::WebRequestDelegateJob( const WebRequestDelegateConnectionRef& conn_, const WebRequestDelegateDataRef& data_, CurlEventLoop* eventloop_)
	:CurlEventLoop::Job(conn_->handle())
	,m_conn(conn_),m_data(data_),m_eventloop(eventloop_)
	,m_response_content()
{
	m_response_errbuf[0] = '\0';
	CURL* curl = conn_->handle();

	set_curl_opt( curl, CURLOPT_POSTFIELDSIZE, m_data->content.size());
	set_curl_opt( curl, CURLOPT_POSTFIELDS, m_data->content.c_str());
	set_curl_opt( curl, CURLOPT_WRITEDATA, &m_response_content);
	set_curl_opt( curl, CURLOPT_WRITEFUNCTION, std_string_append_callback); 
	set_curl_opt( curl, CURLOPT_FAILONERROR, 0);
	set_curl_opt( curl, CURLOPT_ERRORBUFFER, m_response_errbuf);
	set_curl_opt( curl, CURLOPT_CUSTOMREQUEST, m_data->method.c_str());
}

void WebRequestDelegateJob::resume( CURLcode ec)
{
	try
	{
		CURL* curl = m_conn->handle();
		if (ec == CURLE_OK)
		{
			long http_code = 0;
			curl_easy_getinfo( curl, CURLINFO_RESPONSE_CODE, &http_code);
			WebRequestContent answerContent( "UTF-8", "application/json", m_response_content.c_str(), m_response_content.size());
			WebRequestAnswer answer( "", http_code, 0/*apperrorcode*/, answerContent);
			m_data->receiver->putAnswer( answer);
			m_conn->done();
		}
		else
		{
			long http_code = 500;
			curl_easy_getinfo( curl, CURLINFO_RESPONSE_CODE, &http_code);
			WebRequestAnswer answer( m_response_errbuf, http_code, ErrorCodeDelegateRequestFailed);
			m_data->receiver->putAnswer( answer);
			m_conn->reconnect();
		}
		WebRequestDelegateDataRef next = m_conn->fetch();
		if (next.get())
		{
			(void)m_eventloop->pushJob( new WebRequestDelegateJob( m_conn, next, m_eventloop));
		}
	}
	catch (const std::runtime_error& err)
	{
		m_eventloop->handleException( err.what());
		m_conn->dropAllPendingRequests( err.what());
	}
	catch (const std::bad_alloc& )
	{
		m_eventloop->handleException( _TXT("memory allocation error"));
		m_conn->dropAllPendingRequests( _TXT("memory allocation error"));
	}
	catch (...)
	{
		m_eventloop->handleException( _TXT("unexpected exception"));
		m_conn->dropAllPendingRequests( _TXT("unexpected exception"));
	}
}

WebRequestDelegateConnectionRef WebRequestDelegateConnectionPool::getConnection( const std::string& address)
{
	strus::unique_lock lock( m_mutex);
	ConnectionMap::const_iterator ci = m_connectionMap.find( address);
	return ci == m_connectionMap.end() ? WebRequestDelegateConnectionRef() : ci->second;
}

void WebRequestDelegateConnectionPool::setConnection( const std::string& address, const WebRequestDelegateConnectionRef& ref)
{
	strus::unique_lock lock( m_mutex);
	m_connectionMap[ address] = ref;
}

WebRequestDelegateConnectionRef WebRequestDelegateConnectionPool::getOrCreateConnection( const std::string& address)
{
	WebRequestDelegateConnectionRef rt = getConnection( address);
	if (!rt.get())
	{
		rt.reset( new WebRequestDelegateConnection( address, m_eventloop));
		setConnection( address, rt);
	}
	return rt;
}

void WebRequestDelegateConnectionPool::send( const std::string& address_, const std::string& method_, const std::string& content_, const strus::shared_ptr<WebRequestDelegateContextInterface>& receiver_)
{
	WebRequestDelegateConnectionRef conn;
	try
	{
		conn = getOrCreateConnection( address_);
		conn->push( method_, content_, receiver_);
		WebRequestDelegateDataRef data( conn->fetch());
		if (data.get())
		{
			m_eventloop->pushJob( new WebRequestDelegateJob( conn, data, m_eventloop));
		}
	}
	catch (const std::runtime_error& err)
	{
		WebRequestAnswer answer( err.what(), 500, ErrorCodeDelegateRequestFailed);
		receiver_->putAnswer( answer);
		m_eventloop->handleException( err.what());
		if (conn.get()) conn->dropAllPendingRequests(  err.what());
	}
	catch (const std::bad_alloc& )
	{
		WebRequestAnswer answer( _TXT("memory allocation error"), 500, ErrorCodeOutOfMem);
		receiver_->putAnswer( answer);
		m_eventloop->handleException( _TXT("memory allocation error"));
		if (conn.get()) conn->dropAllPendingRequests( _TXT("memory allocation error"));
	}
	catch (...)
	{
		WebRequestAnswer answer( _TXT("unexpected exception"), 500, ErrorCodeDelegateRequestFailed);
		receiver_->putAnswer( answer);
		m_eventloop->handleException( _TXT("unexpected exception"));
		if (conn.get()) conn->dropAllPendingRequests( _TXT("unexpected exception"));
	}
}


