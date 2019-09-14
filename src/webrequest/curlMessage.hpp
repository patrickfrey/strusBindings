/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Message to process (send/receive) with CURL
/// \file "curlMessage.hpp"
#ifndef _STRUS_BINDINGS_CURL_MESSAGE_HPP_INCLUDED
#define _STRUS_BINDINGS_CURL_MESSAGE_HPP_INCLUDED
#include "strus/reference.hpp"
#include "strus/base/filehandle.hpp"
#include "curlLogger.hpp"
#include <string>
#include <curl/curl.h>

namespace strus {

/// \brief Forward declaration
class WebRequestLoggerInterface;

/// \brief Message to process (send/receive) with CURL
class CurlMessage
{
public:
	CurlMessage();
	CurlMessage( const std::string& address, const std::string& method_, const std::string& content_, CurlLogger* logger_);
	~CurlMessage();

	CURL* handle() const
	{
		return m_curl;
	}

	void flushLogs();

	const std::string& response_content() const	{return m_response_content;}
	const char* response_error() const		{return m_response_errbuf[0] ? m_response_errbuf : NULL;}

private:
	CurlMessage( const CurlMessage&){}	//... non copyable
	void operator=( const CurlMessage&){}	//... non copyable

private:
	CURL* m_curl;
	CurlLogger* m_logger;
	std::string m_method;
	std::string m_url;
	int m_port;
	strus::Reference<strus::WriteBufferHandle> m_curlLogBuf;
	std::string m_content;
	std::string m_response_content;
	char m_response_errbuf[ CURL_ERROR_SIZE];
};

}//namespace
#endif


