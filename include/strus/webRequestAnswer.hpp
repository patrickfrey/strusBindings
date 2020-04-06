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
#include "strus/reference.hpp"
#include <cstddef>
#include <string>
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <cstdlib>

namespace strus
{

/// \brief Structure holding an answer of a webrequest
class WebRequestAnswer
{
public:
	/// \brief Default constructor
	explicit WebRequestAnswer( int httpStatus_=200)
		:m_errorStr(0),m_httpStatus(httpStatus_),m_appErrorCode(0),m_messageType(0),m_messageStr(0),m_content(),m_memBlock(){m_errorBuf[0]=0;}
	/// \brief Constructor
	/// \param[in] errorStr_ error message string
	/// \param[in] httpStatus_ http status code
	/// \param[in] appErrorCode_ strus application error code
	/// \param[in] content_ content of the answer
	WebRequestAnswer( int httpStatus_, int appErrorCode_, const WebRequestContent& content_)
		:m_errorStr(0),m_httpStatus(httpStatus_),m_appErrorCode(appErrorCode_),m_messageType(0),m_messageStr(0),m_content(content_),m_memBlock(){m_errorBuf[0]=0;}
	/// \brief Constructor
	/// \param[in] content_ content of the answer
	WebRequestAnswer( const WebRequestContent& content_)
		:m_errorStr(0),m_httpStatus(200),m_appErrorCode(0),m_messageType(0),m_messageStr(0),m_content(content_),m_memBlock(){m_errorBuf[0]=0;}
	/// \brief Constructor
	/// \param[in] httpStatus_ http status code
	/// \param[in] content_ content of the answer
	WebRequestAnswer( int httpStatus_, const WebRequestContent& content_)
		:m_errorStr(0),m_httpStatus(httpStatus_),m_appErrorCode(0),m_messageType(0),m_messageStr(0),m_content(content_),m_memBlock(){m_errorBuf[0]=0;}
	/// \brief Constructor
	/// \param[in] errorStr_ error message string
	/// \param[in] httpStatus_ http status code
	/// \param[in] appErrorCode_ strus application error code
	WebRequestAnswer( int httpStatus_, int appErrorCode_, const char* errorStr_, bool doCopy=false)
		:m_errorStr(0),m_httpStatus(0),m_appErrorCode(0),m_messageType(0),m_messageStr(0),m_content(),m_memBlock(){setError(httpStatus_,appErrorCode_,errorStr_,doCopy);}
	/// \brief Copy constructor
	WebRequestAnswer( const WebRequestAnswer& o)
		:m_errorStr(o.m_errorStr),m_httpStatus(o.m_httpStatus),m_appErrorCode(o.m_appErrorCode),m_messageType(o.m_messageType),m_messageStr(o.m_messageStr),m_content(o.m_content),m_memBlock(o.m_memBlock)
	{
		copyErrorBuf( o);
	}
	/// \brief Assignment operator
	WebRequestAnswer& operator=( const WebRequestAnswer& o)
	{
		m_errorStr = o.m_errorStr;
		m_httpStatus = o.m_httpStatus;
		m_appErrorCode = o.m_appErrorCode;
		m_messageType = o.m_messageType;
		m_messageStr = o.m_messageStr;
		m_content = o.m_content;
		m_memBlock = o.m_memBlock;
		copyErrorBuf( o);
		return *this;
	}

	void copyErrorBuf( const WebRequestAnswer& o)
	{
		if (o.m_errorStr == o.m_errorBuf)
		{
			std::memcpy( m_errorBuf, o.m_errorBuf, sizeof(m_errorBuf));
			m_errorStr = m_errorBuf;
		}
		else
		{
			m_errorBuf[0] = 0;
		}
	}

#if __cplusplus >= 201103L
	WebRequestAnswer( WebRequestContent&& o)
		:m_errorStr(0),m_httpStatus(200),m_appErrorCode(0),m_messageType(0),m_messageStr(0),m_content(std::move(o)),m_memBlock(){m_errorBuf[0]=0;}
#endif

	/// \brief Test if request succeeded
	bool ok() const					{return !m_errorStr;}

	/// \brief Error message in case of failure or NULL
	const char* messageStr() const			{return m_messageStr;}
	/// \brief Error message in case of failure or NULL
	const char* messageType() const			{return m_messageType;}
	/// \brief Error message in case of failure or NULL
	const char* errorStr() const			{return m_errorStr;}
	/// \brief HTTP status of the request
	int httpStatus() const				{return m_httpStatus;}
	/// \brief Application error code of the request
	int appErrorCode() const			{return m_appErrorCode;}

	/// \brief Get content of the answer
	const WebRequestContent& content() const	{return m_content;}
	/// \brief Access content of the answer
	WebRequestContent& content()			{return m_content;}

	/// \brief Set http status and reset error
	/// \param[in] httpStatus_ http status code
	void setHttpStatus( int httpStatus_)
	{
		m_errorStr = 0;
		m_httpStatus = httpStatus_;
		m_appErrorCode = 0;
	}

	/// \brief Set http status with error
	/// \param[in] httpStatus_ http status code
	/// \param[in] appErrorCode_ application error code
	/// \param[in] errorStr_ pointer to the error message
	/// \param[in] doCopy is the message should be copied
	void setError( int httpStatus_, int appErrorCode_, const char* errorStr_=0, bool doCopy=false)
	{
		if (doCopy)
		{
			std::size_t len = std::snprintf( m_errorBuf, sizeof(m_errorBuf), "%s", errorStr_);
			if (len >= sizeof(m_errorBuf)) m_errorBuf[ sizeof(m_errorBuf)-1] = 0;
			m_errorStr = m_errorBuf;
		}
		else
		{
			m_errorBuf[ 0] = 0;
			m_errorStr = errorStr_;
		}
		m_httpStatus = httpStatus_;
		m_appErrorCode = appErrorCode_;
	}

	/// \brief Set http status with formatted error message
	/// \param[in] httpStatus_ http status code
	/// \param[in] appErrorCode_ application error code
	/// \param[in] format format string of the error message
	/// \param[in] doCopy is the message should be copied
	void setError_fmt( int httpStatus_, int appErrorCode_, const char* format, ...)
#ifdef __GNUC__
		__attribute__ ((format (printf, 4, 5)))
#endif
	{
		char buf[ 2048];
		va_list ap;
		va_start( ap, format);

		std::size_t len = ::vsnprintf( buf, sizeof(buf), format, ap);
		if (len >= sizeof(buf)) buf[ sizeof(buf)-1] = 0;

		setError( httpStatus_, appErrorCode_, buf, true/*doCopy*/);
		va_end( ap);
	}

	/// \brief Set http status with a message (no error)
	/// \param[in] httpStatus_ http status code
	/// \param[in] type type of the message
	/// \param[in] str pointer to the message (0-terminated string)
	/// \param[in] doCopy is the message should be copied
	void setMessage( int httpStatus_, const char* type, const char* str, bool doCopy=false)
	{
		if (doCopy)
		{
			m_errorStr = 0;
			std::size_t len = std::snprintf( m_errorBuf, sizeof(m_errorBuf), "%s", str);
			if (len >= sizeof(m_errorBuf)) m_errorBuf[ sizeof(m_errorBuf)-1] = 0;
			m_messageStr = m_errorBuf;
		}
		else
		{
			m_messageStr = str;
			m_errorBuf[ 0] = 0;
		}
		m_httpStatus = httpStatus_;
		m_messageType = type;
	}

	/// \brief Set content of answer (shallow copy)
	/// \param[in] content_ content structure
	void setContent( const WebRequestContent& content_)
	{
		m_content = content_;
	}

	/// \brief Define memory block under control of this answer with allocations used for construction
	/// \param[in] mem memory block (with ownership)
	/// \note only one memory block can be attached to an answer
	void defineMemBlock( char* mem)
	{
		if (m_memBlock.get()) {delete mem; throw std::bad_alloc();}
		m_memBlock.reset( mem);
	}

	/// \brief Allocate an own copy the content
	/// \return false on memory allocation error or if content already defined
	bool copyContent()
	{
		if (m_memBlock.get()) return false;
		char* strptr = (char*)std::malloc( m_content.len()+1);
		if (!strptr) return false;
		std::memcpy( strptr, m_content.str(), m_content.len());
		strptr[ m_content.len()] = 0;
		m_memBlock.reset( strptr);
		m_content.setContent( strptr, m_content.len());
		return true;
	}

	/// \brief Add some explanation to error message
	/// \param[in] msg explanation message
	void explain( const char* msg)
	{
		if (m_errorStr == m_errorBuf)
		{
			char tmpbuf[ errorBufSize];
			std::size_t tmpsize = std::snprintf( tmpbuf, sizeof(tmpbuf), "%s: %s", msg, m_errorStr);
			if (tmpsize >= sizeof(tmpbuf)-1)
			{
				tmpsize = sizeof(tmpbuf)-1;
				tmpbuf[ tmpsize] = 0;
			}
			std::memcpy( m_errorBuf, tmpbuf, tmpsize+1);
		}
		else
		{
			std::size_t tmpsize;
			if (m_errorStr)
			{
				tmpsize = std::snprintf( m_errorBuf, sizeof(m_errorBuf), "%s: %s", msg, m_errorStr);
			}
			else
			{
				tmpsize = std::snprintf( m_errorBuf, sizeof(m_errorBuf), "%s: http status %d", msg, m_httpStatus);
			}
			if (tmpsize >= sizeof(m_errorBuf)-1)
			{
				tmpsize = sizeof(m_errorBuf)-1;
				m_errorBuf[ tmpsize] = 0;
			}
		}
		m_errorStr = m_errorBuf;
	}

private:
	class StandardMallocDeleter
	{
	public:
		void operator()( char* ptr) {std::free(ptr);}
	};
	typedef strus::Reference<char,StandardMallocDeleter> MemBlock;

private:
	enum {errorBufSize=4096};
	char m_errorBuf[ errorBufSize];	///< local buffer for error messages
	const char* m_errorStr;		///< error message in case of failure or NULL
	int m_httpStatus;		///< http status of the request
	int m_appErrorCode;		///< application error code of the request answer
	const char* m_messageType;	///< type of the answer for answers returned in the HTTP header
	const char* m_messageStr;	///< string of message
	WebRequestContent m_content;	///< content of the answer
	MemBlock m_memBlock;		///< memory block used for alloctions of memory for this answer
};

}//namespace
#endif

