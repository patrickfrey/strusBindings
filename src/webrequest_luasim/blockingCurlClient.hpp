/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Client in cURL performing blocking calls to other services
/// \file "blockingCurlClient.hpp"
#ifndef _STRUS_BINDINGS_BLOCKING_CURL_CLIENT_HPP_INCLUDED
#define _STRUS_BINDINGS_BLOCKING_CURL_CLIENT_HPP_INCLUDED
#include <string>

namespace strus {

class BlockingCurlClient
{
public:
	BlockingCurlClient();
	~BlockingCurlClient();

	struct Response
	{
		int httpstatus;
		std::string content;

		Response( int httpstatus_, const std::string& content_)
			:httpstatus(httpstatus_),content(content_){}
		Response( const Response& o)
			:httpstatus(o.httpstatus),content(o.content){}
	};

	Response sendJsonUtf8( const std::string& requestMethod, const std::string& url, const std::string& content, int nofConnRetries) const;
};

}//namespace
#endif

