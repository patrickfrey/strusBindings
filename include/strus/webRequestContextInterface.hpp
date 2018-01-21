/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Interface for executing XML/JSON requests on the strus bindings
/// \file "webRequestContextInterface.hpp"
#ifndef _STRUS_WEB_REQUEST_CONTEXT_INTERFACE_HPP_INCLUDED
#define _STRUS_WEB_REQUEST_CONTEXT_INTERFACE_HPP_INCLUDED
#include <cstddef>
#include <string>

namespace strus
{

class WebRequestAnswer
{
public:
	/// \brief Error message in case of failure or NULL
	const char* errorstr() const	{return m_errorstr;}
	/// \brief HTTP status of the request
	int httpstatus() const		{return m_httpstatus;}
	/// \brief Pointer to the answer content string of the request
	const char* str() const		{return m_str;}
	/// \brief Length of the answer in bytes
	std::size_t len() const		{return m_len;}

	WebRequestAnswer( const char* errorstr_, int httpstatus_, const char* str_, std::size_t len_)
		:m_errorstr(errorstr_),m_httpstatus(httpstatus_),m_str(str_),m_len(len_){}
	WebRequestAnswer( const WebRequestAnswer& o)
		:m_errorstr(o.m_errorstr),m_httpstatus(o.m_httpstatus),m_str(o.m_str),m_len(o.m_len){}

private:
	const char* m_errorstr;	///< error message in case of failure or NULL
	int m_httpstatus;		///< http status of the request
	const char* m_str;	///< pointer to the answer content string of the request
	std::size_t m_len;	///< length of the answer in bytes
};


/// \brief Interface for executing an XML/JSON request on the strus bindings
class WebRequestContextInterface
{
public:
	/// \brief Destructor
	virtual ~WebRequestContextInterface(){}

	/// \brief Add a variable of the request
	/// \param[in] name name of the variable
	/// \param[in] value value of the variable
	/// \return true on success, false on failure (get the error with 'lastError()const')
	virtual bool addVariable( const std::string& name, const std::string& value)=0;

	/// \brief Run the request
	/// \param[in] doctype document content type "XML" or "JSON"
	/// \param[in] encoding character set encoding, e.g. "UTF-8' or "UTF-16"  or "UTF-16BE"
	/// \param[in] content pointer to source
	/// \param[in] contentlen length of source in bytes
	/// \return the answer of the request
	virtual WebRequestAnswer execute( const char* doctype, const char* encoding, const char* content, std::size_t contentlen)=0;
};

}//namespace
#endif


