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
#include <cstddef>
#include <string>
#include <cstdio>

namespace strus
{

class WebRequestAnswer
{
public:
	/// \brief Default constructor
	WebRequestAnswer()
		:m_errorstr(0),m_httpstatus(200),m_apperrorcode(0),m_contentstr(0),m_contentlen(0){}
	/// \brief Constructor
	WebRequestAnswer( const char* errorstr_, int httpstatus_, int apperrorcode_, const char* contentstr_, std::size_t contentlen_)
		:m_errorstr(errorstr_),m_httpstatus(httpstatus_),m_apperrorcode(0),m_contentstr(contentstr_),m_contentlen(contentlen_){}
	/// \brief Copy constructor
	WebRequestAnswer( const WebRequestAnswer& o)
		:m_errorstr(o.m_errorstr),m_httpstatus(o.m_httpstatus),m_apperrorcode(o.m_apperrorcode),m_contentstr(o.m_contentstr),m_contentlen(o.m_contentlen){}


	/// \brief Test if request succeeded
	bool ok() const			{return !m_errorstr;}

	/// \brief Error message in case of failure or NULL
	const char* errorstr() const	{return m_errorstr;}
	/// \brief HTTP status of the request
	int httpstatus() const		{return m_httpstatus;}
	/// \brief Application error code of the request
	int apperror() const		{return m_apperrorcode;}
	/// \brief Pointer to the answer content string of the request
	const char* contentstr() const	{return m_contentstr;}
	/// \brief Length of the answer in bytes
	std::size_t contentlen() const	{return m_contentlen;}

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
	/// \brief Set content of answer
	/// \param[in] contentstr_ pointer to content
	/// \param[in] contentlen_ size of content in bytes
	void setContent( const char* contentstr_, std::size_t contentlen_)
	{
		m_contentstr = contentstr_;
		m_contentlen = contentlen_;
	}

private:
	char m_errorbuf[ 1024];		///< local buffer for error messages
	const char* m_errorstr;		///< error message in case of failure or NULL
	int m_httpstatus;		///< http status of the request
	int m_apperrorcode;		///< application error code of the request
	const char* m_contentstr;	///< pointer to the answer content string of the request
	std::size_t m_contentlen;	///< length of the answer in bytes
};

}//namespace
#endif

