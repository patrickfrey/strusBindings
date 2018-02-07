/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Structure holding the data belonging to an answer of an XML/JSON request on the strus bindings
/// \file "webRequestAnswer.hpp"
#ifndef _STRUS_WEB_REQUEST_ANSWER_HPP_INCLUDED
#define _STRUS_WEB_REQUEST_ANSWER_HPP_INCLUDED
#include "webRequestContent.hpp"
#include <cstddef>
#include <string>
#include <cstdio>
#include <cstring>

namespace strus
{

class WebRequestAnswer
{
public:
	/// \brief Default constructor
	WebRequestAnswer()
		:m_errorstr(0),m_httpstatus(200),m_apperrorcode(0),m_content(){}
	/// \brief Constructor
	WebRequestAnswer( const char* errorstr_, int httpstatus_, int apperrorcode_, const WebRequestContent& content_)
		:m_errorstr(errorstr_),m_httpstatus(httpstatus_),m_apperrorcode(0),m_content(content_){}
	/// \brief Constructor
	WebRequestAnswer( const WebRequestContent& content_)
		:m_errorstr(0),m_httpstatus(200),m_apperrorcode(0),m_content(content_){}
	/// \brief Constructor
	WebRequestAnswer( const char* errorstr_, int httpstatus_, int apperrorcode_)
		:m_errorstr(errorstr_),m_httpstatus(httpstatus_),m_apperrorcode(0),m_content(){}
	/// \brief Copy constructor
	WebRequestAnswer( const WebRequestAnswer& o)
		:m_errorstr(o.m_errorstr),m_httpstatus(o.m_httpstatus),m_apperrorcode(o.m_apperrorcode),m_content(o.m_content)
	{
		if (o.m_errorstr == o.m_errorbuf)
		{
			std::memcpy( m_errorbuf, o.m_errorbuf, sizeof(m_errorbuf));
			m_errorstr = m_errorbuf;
		}
		else
		{
			m_errorbuf[0] = 0;
		}
	}

	/// \brief Test if request succeeded
	bool ok() const			{return !m_errorstr;}

	/// \brief Error message in case of failure or NULL
	const char* errorstr() const			{return m_errorstr;}
	/// \brief HTTP status of the request
	int httpstatus() const				{return m_httpstatus;}
	/// \brief Application error code of the request
	int apperror() const				{return m_apperrorcode;}

	/// \brief content of the answer
	const WebRequestContent& content() const	{return m_content;}

	/// \brief Set http status and reset error
	/// \param[in] httpstatus_ http status code
	void setStatus( int httpstatus_)
	{
		m_errorstr = 0;
		m_httpstatus = httpstatus_;
		m_apperrorcode = 0;
	}
	/// \brief Set http status with error
	/// \param[in] httpstatus_ http status code
	/// \param[in] apperrorcode_ application error code
	/// \param[in] errorstr_ pointer to error message
	void setError( int httpstatus_, int apperrorcode_, const char* errorstr_, bool doCopy=false)
	{
		if (doCopy)
		{
			std::size_t len = std::snprintf( m_errorbuf, sizeof(m_errorbuf), "%s", errorstr_);
			if (len >= sizeof(m_errorbuf)) m_errorbuf[ sizeof(m_errorbuf)-1] = 0;
			m_errorstr = m_errorbuf;
		}
		else
		{
			m_errorstr = errorstr_;
		}
		m_httpstatus = httpstatus_;
		m_apperrorcode = apperrorcode_;
	}
	/// \brief Set content of answer (shallow copy)
	/// \param[in] content_ content structure
	void setContent( const WebRequestContent& content_)
	{
		m_content = content_;
	}

private:
	char m_errorbuf[ 1024];		///< local buffer for error messages
	const char* m_errorstr;		///< error message in case of failure or NULL
	int m_httpstatus;		///< http status of the request
	int m_apperrorcode;		///< application error code of the request answer
	WebRequestContent m_content;	///< content of the answer
};

}//namespace
#endif

