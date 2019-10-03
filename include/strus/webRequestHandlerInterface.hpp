/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Interface for executing XML/JSON requests based on the strus bindings API
/// \file "webRequestHandlerInterface.hpp"
#ifndef _STRUS_WEB_REQUEST_HANDLER_INTERFACE_HPP_INCLUDED
#define _STRUS_WEB_REQUEST_HANDLER_INTERFACE_HPP_INCLUDED
#include "strus/webRequestAnswer.hpp"
#include "strus/webRequestContent.hpp"
#include <cstddef>
#include <vector>
#include <string>

namespace strus
{

/// \brief Forward declaration
class WebRequestContextInterface;
/// \brief Forward declaration
class WebRequestDelegateContextInterface;

/// \brief Interface for executing XML/JSON requests based on the strus bindings API
class WebRequestHandlerInterface
{
public:
	/// \brief Destructor
	virtual ~WebRequestHandlerInterface(){}

	/// \brief Load configuration, do some initializations, cleanup and organization
	/// \note Has to be called before any other method of the interface 
	/// \param[in] configSource source of configuration to load
	/// \param[out] answer the error status
	/// \return true on success, false on failure
	virtual bool init( 
			const std::string& configsrc,
			WebRequestAnswer& answer)=0;

	/// \brief Create the structure for handling a configuration request
	/// \note A configuration request is defined as embedded section in the configuration file and it adresses its object not by the path but explicitely addresses the object by type and name.
	/// \note It does not store any artifacts for recreation on restart because it does not have to remember any objects defined as they are defined in the main configuration file.
	/// \note It is not recommended to use this context except for creation of the main context. Do not use embedded configurations of other objects, if you want to build bigger systems.
	/// \param[in] contextType context type
	/// \param[in] contextName context name
	/// \param[out] answer the error status
	/// \return the context structure for handling the configuration request or NULL in case of an error (inspect answer for the error details)
	virtual WebRequestContextInterface* createConfigurationContext(
			const char* contextType,
			const char* contextName,
			WebRequestAnswer& answer)=0;

	/// \brief Create the structure for handling a request
	/// \param[in] accepted_charset HTTP header variable 'Accept-Charset', a comma separated list of character set encodings accepted by the client
	/// \param[in] accepted_doctype HTTP header variable 'Accept', a comma separated list of content types accepted by the client
	/// \param[in] html_base_href base link for HTML href links (base in HTML head)
	/// \param[in] method request method in uppercase
	/// \param[in] path path of the request
	/// \param[out] answer the error status
	/// \return the context structure for handling a request or NULL in case of an error (inspect answer for the error details)
	virtual WebRequestContextInterface* createRequestContext(
			const char* accepted_charset,
			const char* accepted_doctype,
			const char* html_base_href,
			const char* method,
			const char* path,
			WebRequestAnswer& answer)=0;

	/// \brief Send a request to another server and get notified by a callback
	/// \param[in] address where to send the request
	/// \param[in] method request method of the request
	/// \param[in] content data of the request (content type is "application/json; charset=utf-8")
	/// \param[in] context pointer to object processing the answer of the request (with ownership)
	virtual bool delegateRequest(
			const std::string& address,
			const std::string& method,
			const std::string& content,
			WebRequestDelegateContextInterface* context)=0;

	/// \brief Get an answer structure for a simple message string that does not need a context to be defined
	/// \param[in] name key used for the message in a markup language
	/// \param[in] message content string for the message
	/// \return answer answer structure (as error if an error occurred)
	virtual WebRequestAnswer getSimpleRequestAnswer(
			const char* accepted_charset,
			const char* accepted_doctype,
			const char* html_base_href,
			const std::string& name,
			const std::string& message)=0;
};

}//namespace
#endif


