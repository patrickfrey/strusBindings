/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Structure holding the data belonging to an answer of an XML/JSON request on the strus bindings
/// \file "webRequestContent.hpp"
#ifndef _STRUS_WEB_REQUEST_CONTENT_HPP_INCLUDED
#define _STRUS_WEB_REQUEST_CONTENT_HPP_INCLUDED
#include <cstddef>
#include <string>
#include <cstring>
#include <cstdio>

namespace strus
{

class WebRequestContent
{
public:
	/*
	 * @brief Request content type enumeration
	 */
	enum Type {
		Unknown,		///< Content type is not known */
		XML,			///< Content type is XML*/
		JSON,			///< Content type is JSON */
		HTML,			///< Content type is HTML */
		TEXT			///< Content type is TEXT */
	};

	/// \brief Get the content type name as used for MIME
	/// \param[in] type the content type id
	/// \return content type MIME name string
	static const char* typeMime( Type type)
	{
		static const char* ar[] = {"application/octet-stream","application/xml","application/json","text/html","application/xhtml","text/plain"};
		return ar[ type];
	}

	/// \brief Get the content type name as string
	/// \param[in] type the content type id
	/// \return content type name string
	static const char* typeName( Type type)
	{
		static const char* ar[] = {"unknown","XML","JSON","HTML","XHTML","TEXT"};
		return ar[ type];
	}

public:
	/// \brief Default constructor
	WebRequestContent()
		:m_charset(0),m_doctype(0),m_str(0),m_len(0){}
	/// \brief Constructor
	WebRequestContent( const char* charset_, const char* doctype_, const char* str_, std::size_t len_)
		:m_charset(charset_),m_doctype(doctype_),m_str(str_),m_len(len_){}
	/// \brief Copy constructor
	WebRequestContent( const WebRequestContent& o)
		:m_charset(o.m_charset),m_doctype(o.m_doctype),m_str(o.m_str),m_len(o.m_len){}

	/// \brief Pointer to the character set encoding identifier of this content
	const char* charset() const	{return m_charset;}
	/// \brief Pointer to the document type identifier of this content
	const char* doctype() const	{return m_doctype;}
	/// \brief Pointer to the answer content string of the request
	const char* str() const		{return m_str;}
	/// \brief Length of the answer in bytes
	std::size_t len() const		{return m_len;}

	/// \brief Set content of answer
	/// \param[in] contentstr_ pointer to content
	/// \param[in] contentlen_ size of content in bytes
	void setContent( const char* contentstr_, std::size_t contentlen_)
	{
		m_str = contentstr_;
		m_len = contentlen_;
	}
	/// \brief Set the character set encoding of this answer
	/// \param[in] charset_ pointer to character set encoding identifier
	void setCharset( const char* charset_)
	{
		m_charset = charset_;
	}
	/// \brief Set the document type of this answer
	/// \param[in] doctype_ pointer to document type identifier
	void setDoctype( const char* doctype_)
	{
		m_doctype = doctype_;
	}

private:
	const char* m_charset;	///< character set encoding, e.g. "UTF-8", "UTF-16BE", ...
	const char* m_doctype;	///< document type, e.g. "application/xml", "application/json", "text/plain", ...
	const char* m_str;	///< pointer to the answer content string of the request
	std::size_t m_len;	///< length of the answer in bytes
};

}//namespace
#endif

