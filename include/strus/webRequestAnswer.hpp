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

/// \brief Structure holding an answer of a webrequest
class WebRequestAnswer
{
public:
	/// \brief Default constructor
	WebRequestAnswer()
		:m_errorstr(0),m_httpstatus(200),m_apperrorcode(0),m_messagetype(0),m_messagestr(0),m_content(){m_errorbuf[0]=0;}
	/// \brief Constructor
	WebRequestAnswer( const char* errorstr_, int httpstatus_, int apperrorcode_, const WebRequestContent& content_)
		:m_errorstr(errorstr_),m_httpstatus(httpstatus_),m_apperrorcode(0),m_messagetype(0),m_messagestr(0),m_content(content_){m_errorbuf[0]=0;}
	/// \brief Constructor
	WebRequestAnswer( const WebRequestContent& content_)
		:m_errorstr(0),m_httpstatus(200),m_apperrorcode(0),m_messagetype(0),m_messagestr(0),m_content(content_){m_errorbuf[0]=0;}
	/// \brief Constructor
	WebRequestAnswer( int httpstatus_, const WebRequestContent& content_)
		:m_errorstr(0),m_httpstatus(httpstatus_),m_apperrorcode(0),m_messagetype(0),m_messagestr(0),m_content(content_){m_errorbuf[0]=0;}
	/// \brief Constructor
	WebRequestAnswer( const char* errorstr_, int httpstatus_, int apperrorcode_)
		:m_errorstr(errorstr_),m_httpstatus(httpstatus_),m_apperrorcode(0),m_messagetype(0),m_messagestr(0),m_content(){m_errorbuf[0]=0;}
	/// \brief Copy constructor
	WebRequestAnswer( const WebRequestAnswer& o)
		:m_errorstr(o.m_errorstr),m_httpstatus(o.m_httpstatus),m_apperrorcode(o.m_apperrorcode),m_messagetype(o.m_messagetype),m_messagestr(o.m_messagestr),m_content(o.m_content)
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
	WebRequestAnswer& operator=( const WebRequestAnswer& o)
	{
		m_errorstr = o.m_errorstr;
		m_httpstatus = o.m_httpstatus;
		m_apperrorcode = o.m_apperrorcode;
		m_messagetype = o.m_messagetype;
		m_messagestr = o.m_messagestr;
		m_content = o.m_content;
		if (o.m_errorstr == o.m_errorbuf)
		{
			std::memcpy( m_errorbuf, o.m_errorbuf, sizeof(m_errorbuf));
			m_errorstr = m_errorbuf;
		}
		else
		{
			m_errorbuf[0] = 0;
		}
		return *this;
	}

	/// \brief Test if request succeeded
	bool ok() const					{return !m_errorstr;}

	/// \brief Error message in case of failure or NULL
	const char* messagestr() const			{return m_messagestr;}
	/// \brief Error message in case of failure or NULL
	const char* messagetype() const			{return m_messagetype;}
	/// \brief Error message in case of failure or NULL
	const char* errorstr() const			{return m_errorstr;}
	/// \brief HTTP status of the request
	int httpstatus() const				{return m_httpstatus;}
	/// \brief Application error code of the request
	int apperror() const				{return m_apperrorcode;}

	/// \brief Get content of the answer
	const WebRequestContent& content() const	{return m_content;}
	/// \brief Access content of the answer
	WebRequestContent& content()			{return m_content;}

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
	/// \param[in] errorstr_ pointer to the error message
	/// \param[in] doCopy is the message should be copied
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
	/// \brief Set http status with a message (no error)
	/// \param[in] httpstatus_ http status code
	/// \param[in] type type of the message
	/// \param[in] str pointer to the message (0-terminated string)
	/// \param[in] doCopy is the message should be copied
	void setMessage( int httpstatus_, const char* type, const char* str, bool doCopy=false)
	{
		if (doCopy)
		{
			m_errorstr = 0;
			std::size_t len = std::snprintf( m_errorbuf, sizeof(m_errorbuf), "%s", str);
			if (len >= sizeof(m_errorbuf)) m_errorbuf[ sizeof(m_errorbuf)-1] = 0;
			m_messagestr = m_errorbuf;
		}
		else
		{
			m_messagestr = str;
		}
		m_httpstatus = httpstatus_;
		m_messagetype = type;
	}

	/// \brief Set content of answer (shallow copy)
	/// \param[in] content_ content structure
	void setContent( const WebRequestContent& content_)
	{
		m_content = content_;
	}

	/// \brief Add some explanation to error message
	/// \param[in] msg explanation message
	void explain( const char* msg)
	{
		if (m_errorstr == m_errorbuf)
		{
			char tmpbuf[ errorbufsize];
			std::size_t tmpsize = std::snprintf( tmpbuf, sizeof(tmpbuf), "%s: %s", msg, m_errorstr);
			if (tmpsize >= sizeof(tmpbuf)-1)
			{
				tmpsize = sizeof(tmpbuf)-1;
				tmpbuf[ tmpsize] = 0;
			}
			std::memcpy( m_errorbuf, tmpbuf, tmpsize+1);
		}
		else
		{
			std::size_t tmpsize;
			if (m_errorstr)
			{
				tmpsize = std::snprintf( m_errorbuf, sizeof(m_errorbuf), "%s: %s", msg, m_errorstr);
			}
			else
			{
				tmpsize = std::snprintf( m_errorbuf, sizeof(m_errorbuf), "%s: http status %d", msg, m_httpstatus);
			}
			if (tmpsize >= sizeof(m_errorbuf)-1)
			{
				tmpsize = sizeof(m_errorbuf)-1;
				m_errorbuf[ tmpsize] = 0;
			}
		}
		m_errorstr = m_errorbuf;
	}

private:
	enum {errorbufsize=4096};
	char m_errorbuf[ errorbufsize];	///< local buffer for error messages
	const char* m_errorstr;		///< error message in case of failure or NULL
	int m_httpstatus;		///< http status of the request
	int m_apperrorcode;		///< application error code of the request answer
	const char* m_messagetype;	///< type of the answer for answers returned in the HTTP header
	const char* m_messagestr;	///< string of message
	WebRequestContent m_content;	///< content of the answer
};

}//namespace
#endif

