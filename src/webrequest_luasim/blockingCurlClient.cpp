/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Client in cURL performing blocking calls to other services
/// \file "blockingCurlClient.cpp"
#include "blockingCurlClient.hpp"
#include "strus/base/string_format.hpp"
#include <curl/curl.h>
#include <stdexcept>
#include <cstring>

using namespace strus;

template <typename DATA>
static void set_curl_opt( CURL *curl, CURLoption opt, const DATA& data)
{
	CURLcode res = curl_easy_setopt( curl, opt, data);
	if (res != CURLE_OK)
	{
		throw std::runtime_error( strus::string_format( "failed set socket option: %s", curl_easy_strerror(res)));
	}
}

static void set_http_header( struct curl_slist*& headers, const char* name, const std::string& value)
{
	char buf[1024];
	if ((int)sizeof(buf) < std::snprintf( buf, sizeof(buf), "%s: %s", name, value.c_str())) throw std::bad_alloc();
	headers = curl_slist_append( headers, buf);
	if (!headers) throw std::bad_alloc();
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


struct ResponseBuf
{
	int httpstatus;
	char errbuf[ CURL_ERROR_SIZE];
	std::string content;

	ResponseBuf()
	{
		errbuf[0] = '\0';
		httpstatus = 0;
	}
	void cutDuplicateEolnAtEndOfContent()
	{
		while (content.size() > 2 && content[ content.size()-1] == '\n' && content[ content.size()-2] == '\n')
		{
			content.resize( content.size()-1);
		}
	}
};

static void issueRequest( const std::string& url, int port, const std::string& method_, const std::string& content, ResponseBuf& response)
{
	std::string errout;
	std::string method = method_;
	std::string::iterator mi = method.begin(), me = method.end();
	for (; mi != me; ++mi) *mi = ::toupper( *mi);
	const char* accepted_charset = "UTF-8";
	const char* accepted_doctype = "application/json";
	std::string content_type = strus::string_format( "%s; charset=%s", accepted_doctype, accepted_charset);
	std::string user_agent = strus::string_format( "libcurl/%s",curl_version_info(CURLVERSION_NOW)->version);
	if (content_type.empty() || user_agent.empty()) throw std::bad_alloc();

	CURL *curl = 0;
	CURLcode ec = CURLE_OK;
	struct curl_slist* headers = 0;

	curl = curl_easy_init();
	if (curl) try
	{
		set_curl_opt( curl, CURLOPT_USERAGENT, user_agent.c_str());

		set_http_header( headers, "Expect", "");
		set_http_header( headers, "Content-Type", content_type.c_str());
		set_http_header( headers, "Accept", content_type.c_str());
		set_http_header( headers, "Accept-Charset", accepted_charset);
		set_curl_opt( curl, CURLOPT_POST, 1);
		set_curl_opt( curl, CURLOPT_HTTPHEADER, headers);
		set_curl_opt( curl, CURLOPT_POSTFIELDSIZE, content.size());
		set_curl_opt( curl, CURLOPT_POSTFIELDS, content.c_str());
		set_curl_opt( curl, CURLOPT_WRITEDATA, &response.content);
		set_curl_opt( curl, CURLOPT_WRITEFUNCTION, write_callback); 
		set_curl_opt( curl, CURLOPT_FAILONERROR, 0);
		set_curl_opt( curl, CURLOPT_ERRORBUFFER, response.errbuf);
		set_curl_opt( curl, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_1_1);

		set_curl_opt( curl, CURLOPT_URL, url.c_str());
		set_curl_opt( curl, CURLOPT_CUSTOMREQUEST, method.c_str());
		if (port) set_curl_opt( curl, CURLOPT_PORT, port);

		ec = curl_easy_perform( curl);
		if (ec == CURLE_OK)
		{
			long http_code = 0;
			curl_easy_getinfo( curl, CURLINFO_RESPONSE_CODE, &http_code);
			response.httpstatus = http_code;
		}
	}
	catch (const std::bad_alloc& err)
	{
		ec = CURLE_OUT_OF_MEMORY;
	}
	if (headers) curl_slist_free_all( headers);
	if (curl) curl_easy_cleanup( curl);
	if (ec != CURLE_OK) throw std::runtime_error( curl_easy_strerror( ec));
}

static int getPort( const std::string& url)
{
	char const* ri = std::strchr( url.c_str(), ':');
	if (ri && ri[1] == '/') ri = std::strchr( ri+1, ':');
	return ri ? atoi( ri+1) : 0;
}

static std::string getUrl( const std::string& url)
{
	char const* ri = std::strchr( url.c_str(), ':');
	if (ri && ri[1] == '/') ri = std::strchr( ri+1, ':');
	if (ri)
	{
		std::string rt;
		rt.append( url.c_str(), ri-url.c_str());
		for (++ri; *ri >= '0' && *ri <= '9'; ++ri){}
		rt.append( ri);
		return rt;
	}
	else
	{
		return url;
	}
}

BlockingCurlClient::BlockingCurlClient()
{
	curl_global_init( CURL_GLOBAL_ALL);
}

BlockingCurlClient::~BlockingCurlClient()
{
	curl_global_cleanup();
}

BlockingCurlClient::Response BlockingCurlClient::sendJsonUtf8( const std::string& requestMethod, const std::string& address, const std::string& content) const
{
	std::string url = getUrl( address);
	int port = getPort( address);
	ResponseBuf response;

	issueRequest( url, port, requestMethod, content, response);
	response.cutDuplicateEolnAtEndOfContent();
	return Response( response.httpstatus, response.content);
}


