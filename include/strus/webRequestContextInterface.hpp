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
#include "strus/webRequestAnswer.hpp"
#include <cstddef>
#include <string>

namespace strus
{

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
	/// \param[out] answer the data of the answer of the request
	/// \return bool if request succeeded, false else
	virtual bool execute( const char* doctype, const char* encoding, const char* content, std::size_t contentlen, WebRequestAnswer& answer)=0;
};

}//namespace
#endif


