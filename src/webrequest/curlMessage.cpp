/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Message to process (send/receive) with CURL
/// \file "curlMessage.cpp"
#include "curlMessage.hpp"
#include "curlLogger.hpp"
#include "strus/base/string_format.hpp"
#include "private/internationalization.hpp"
#include <cstring>
#include <curl/curl.h>

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
	~WebRequestDelegateConnectionGlobals()
	{
		if (headers) curl_slist_free_all( headers);
	}
};

static WebRequestDelegateConnectionGlobals g_delegateRequestGlobals;

static int getPort( char const* address)
{
	char const* ri = std::strchr( address, ':');
	if (ri && ri[1] == '/') ri = std::strchr( ri+1, ':');
	return ri ? atoi( ri+1) : 0;
}

static std::string getUrl( char const* address)
{
	char const* ri = std::strchr( address, ':');
	if (ri && ri[1] == '/') ri = std::strchr( ri+1, ':');
	if (ri)
	{
		std::string rt;
		rt.append( address, ri-address);
		for (++ri; *ri >= '0' && *ri <= '9'; ++ri){}
		rt.append( ri);
		return rt;
	}
	else
	{
		return address;
	}
}

CurlMessage::CurlMessage()
	:m_curl(0),m_logger(0)
	,m_method(),m_url(),m_port(0)
	,m_curlLogBuf(),m_content(),m_response_content()
{}

void CurlMessageErrorHandler( void* logger_, int errno_)
{
	CurlLogger* logger = (CurlLogger*)logger_;
	logger->print( CurlLogger::LogError, _TXT("libcurl logging failed: %s"), ::strerror(errno_));
}

CurlMessage::CurlMessage( const std::string& address, const std::string& method_, const std::string& content_, CurlLogger* logger_)
	:m_curl(curl_easy_init()),m_logger(logger_)
	,m_method(method_),m_url(getUrl(address.c_str())),m_port(getPort(address.c_str()))
	,m_curlLogBuf(),m_content(content_),m_response_content()
{
	m_response_errbuf[0] = '\0';
	if (!g_delegateRequestGlobals.valid)
	{
		throw std::runtime_error(_TXT("fatal: delegate connection globals initialization failed"));
	}
	if (m_curl == NULL) throw std::bad_alloc();
	std::transform( m_method.begin(), m_method.end(), m_method.begin(), ::toupper);

	if (m_content.empty() && m_method == "GET")
	{
		set_curl_opt( m_curl, CURLOPT_HTTPGET, 1);
	}
	else
	{
		set_curl_opt( m_curl, CURLOPT_CUSTOMREQUEST, m_method.c_str());
		set_curl_opt( m_curl, CURLOPT_POST, 1);
		set_curl_opt( m_curl, CURLOPT_POSTFIELDSIZE, m_content.size());
		set_curl_opt( m_curl, CURLOPT_POSTFIELDS, m_content.c_str());
	}
	set_curl_opt( m_curl, CURLOPT_USERAGENT, g_delegateRequestGlobals.user_agent.c_str());
	set_curl_opt( m_curl, CURLOPT_HTTPHEADER, g_delegateRequestGlobals.headers);
	set_curl_opt( m_curl, CURLOPT_FAILONERROR, 0);
	set_curl_opt( m_curl, CURLOPT_ERRORBUFFER, m_response_errbuf);
	set_curl_opt( m_curl, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_1_1);
	set_curl_opt( m_curl, CURLOPT_URL, m_url.c_str());
	if (m_port) set_curl_opt( m_curl, CURLOPT_PORT, m_port);
	set_curl_opt( m_curl, CURLOPT_WRITEDATA, &m_response_content);
	set_curl_opt( m_curl, CURLOPT_WRITEFUNCTION, std_string_append_callback); 

	if (m_logger && CurlLogger::LogInfo <= m_logger->loglevel())
	{
		m_curlLogBuf.reset( new strus::WriteBufferHandle( CurlMessageErrorHandler, m_logger));
		if (m_curlLogBuf->error() == 0)
		{
			set_curl_opt( m_curl, CURLOPT_STDERR, m_curlLogBuf->getCStreamHandle());
			//set_curl_opt( m_curl, CURLOPT_STDERR, stderr);
			set_curl_opt( m_curl, CURLOPT_VERBOSE, 1);
		}
		else
		{
			curl_easy_cleanup( m_curl);
			throw strus::runtime_error(_TXT("initialization of stream for verbose logging of cURL failed: %s"), ::strerror(m_curlLogBuf->error()));
		}
	}
}

CurlMessage::~CurlMessage()
{
	if (m_curl) curl_easy_cleanup( m_curl);
}

void CurlMessage::flushLogs()
{
	if (m_curlLogBuf.get())
	{
		std::string conninfo = m_curlLogBuf->fetchContent();
		if (!conninfo.empty())
		{
			if (m_port > 0)
			{
				m_logger->print( CurlLogger::LogInfo, _TXT("libcurl for '%s:%u': %s"), m_url.c_str(), (unsigned int)m_port, conninfo.c_str());
			}
			else
			{
				m_logger->print( CurlLogger::LogInfo, _TXT("libcurl for '%s': %s"), m_url.c_str(), conninfo.c_str());
			}
		}
	}
}


