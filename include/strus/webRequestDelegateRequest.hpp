/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Structure holding the data belonging to a JSON request to another web server as sub request
/// \file "webRequestDelegateRequest.hpp"
#ifndef _STRUS_WEB_REQUEST_DELEGATE_REQUEST_HPP_INCLUDED
#define _STRUS_WEB_REQUEST_DELEGATE_REQUEST_HPP_INCLUDED
#include <cstddef>
#include <string>
#include <cstdio>
#include <cstring>

namespace strus
{

/// \brief Structure holding an answer of a webrequest
class WebRequestDelegateRequest
{
public:
	/// \brief Default constructor
	WebRequestDelegateRequest()
		:m_method(0),m_url(0),m_schema(0),m_receiverType(0),m_receiverName(0),m_contentstr(),m_contentlen(){}
	/// \brief Constructor
	WebRequestDelegateRequest( const char* method_, const char* url_, const char* schema_, const char* contentstr_, std::size_t contentlen_)
		:m_method(method_),m_url(url_),m_schema(schema_),m_receiverType(0),m_receiverName(0),m_contentstr(contentstr_),m_contentlen(contentlen_){}
	/// \brief Copy constructor
	WebRequestDelegateRequest( const WebRequestDelegateRequest& o)
		:m_method(o.m_method),m_url(o.m_url),m_schema(o.m_schema),m_receiverType(o.m_receiverType),m_receiverName(o.m_receiverName),m_contentstr(o.m_contentstr),m_contentlen(o.m_contentlen)
	{}
	WebRequestDelegateRequest& operator=( const WebRequestDelegateRequest& o)
	{
		m_method = o.m_method;
		m_url = o.m_url;
		m_schema = o.m_schema;
		m_receiverType = o.m_receiverType;
		m_receiverName = o.m_receiverName;
		m_contentstr = o.m_contentstr;
		m_contentlen = o.m_contentlen;
		return *this;
	}
	/// \brief Declare receiver for an update of a configuration by this request
	/// \param[in] receiverType_ type of the receiver handling the answer in case of a delegate request decoupled from a pending request (configuration)
	/// \param[in] receiverName_ name of the receiver handling the answer in case of a delegate request decoupled from a pending request (configuration)
	void setReceiver( const char* receiverType_, const char* receiverName_)
	{
		m_receiverType = receiverType_;
		m_receiverName = receiverName_;
	}

	/// \brief Request method
	const char* method() const			{return m_method;}
	/// \brief Get url of the web service to call
	const char* url() const				{return m_url;}
	/// \brief Get name of the schema handling the answer
	const char* schema() const			{return m_schema;}
	/// \brief Get type of the receiver handling the answer in case of a delegate request decoupled from a pending request (configuration)
	const char* receiverType() const		{return m_receiverType;}
	/// \brief Get name of the receiver handling the answer in case of a delegate request decoupled from a pending request (configuration)
	const char* receiverName() const		{return m_receiverType;}
	/// \brief Get content of the delegate request
	const char* contentstr() const			{return m_contentstr;}
	/// \brief Get content length of the delegate request
	std::size_t contentlen() const			{return m_contentlen;}

	/// \brief Set content of answer (shallow copy)
	/// \param[in] content_ content structure
	void setContent( const char* contentstr_, std::size_t contentlen_)
	{
		m_contentstr = contentstr_;
		m_contentlen = contentlen_;
	}

private:
	const char* m_method;		///< request method
	const char* m_url;		///< url of the web service to call
	const char* m_schema;		///< name of the schema handling the answer
	const char* m_receiverType;	///< type of the receiver handling the answer in case of a delegate request decoupled from a pending request (configuration)
	const char* m_receiverName;	///< name of the receiver handling the answer in case of a delegate request decoupled from a pending request (configuration)
	const char* m_contentstr;	///< content of the delegate request (application/json; charser=UTF-8)
	std::size_t m_contentlen;	///< length of the content of the delegate request in bytes
};

}//namespace
#endif

