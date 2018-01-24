/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Implementation of the interface for executing XML/JSON requests on the strus bindings
/// \file "webRequestContext.hpp"
#ifndef _STRUS_WEB_REQUEST_CONTEXT_IMPL_HPP_INCLUDED
#define _STRUS_WEB_REQUEST_CONTEXT_IMPL_HPP_INCLUDED
#include "strus/webRequestContextInterface.hpp"
#include "papuga/requestHandler.h"
#include "papuga/requestParser.h"
#include "papuga/typedefs.h"
#include <stdexcept>

namespace strus
{

/// \brief Implementation of the interface for executing an XML/JSON request on the strus bindings
class WebRequestContext
	:public WebRequestContextInterface
{
public:
	/// \brief Exception thrown by constructor of WebRequestContext
	class Exception
		:public std::runtime_error
	{
	public:
		explicit Exception( papuga_ErrorCode errcode_)
			:std::runtime_error("WebRequestContextException"),m_errcode(errcode_){}
		papuga_ErrorCode errcode() const	{return m_errcode;}
	
	private:
		papuga_ErrorCode m_errcode;
	};

	WebRequestContext(
		papuga_RequestHandler* handlerimpl,
		const char* schema,
		const char* role);
	virtual ~WebRequestContext();

	virtual bool addVariable( const std::string& name, const std::string& value);

	virtual bool execute( const char* doctype, const char* encoding, const char* content, std::size_t contentlen, WebRequestAnswer& answer);

	virtual bool mapError( char* buf, std::size_t bufsize, std::size_t* len, const WebRequestAnswer& answer);

private:
	void clearContent();

private:
	papuga_RequestContext m_impl;
	papuga_Request* m_request;
	papuga_StringEncoding m_encoding;
	papuga_ContentType m_doctype;
	papuga_ErrorCode m_errcode;
	papuga_ErrorBuffer m_errbuf;
	const papuga_RequestAutomaton* m_atm;
	char* m_resultstr;
	std::size_t m_resultlen;
	char m_errbuf_mem[ 4096];
};

}//namespace
#endif


