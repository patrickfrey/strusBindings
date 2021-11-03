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
#include <cstdint>
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
			char const* configsrc,
			size_t configlen,
			WebRequestAnswer& answer)=0;

	/// \brief Complete the part of configuration that connects to other services
	/// \return true if all is ok, false if an error occurred
	/// \remark Errors are logged. But an error does not lead to an interrruption of processing.
	virtual bool synchronize()=0;

	/// \brief Create the structure for handling a request
	/// \param[in] http_accept HTTP header variable 'Accept', a comma separated list of content types accepted by the client
	/// \param[in] html_base_href base link for HTML href links (base in HTML head)
	/// \param[in] method request method in uppercase
	/// \param[in] path path of the request
	/// \param[out] answer the error status
	/// \return the context structure for handling a request or NULL in case of an error (inspect answer for the error details)
	virtual WebRequestContextInterface* createContext(
			char const* http_accept,
			char const* html_base_href,
			char const* method,
			char const* path,
			char const* contentstr,
			size_t contentlen,
			WebRequestAnswer& answer)=0;

	/// \brief Send a request to another server and get notified by a callback
	/// \param[in] address where to send the request
	/// \param[in] method request method of the request
	/// \param[in] content data of the request (content type is "application/json; charset=utf-8")
	/// \param[in] context pointer to object processing the answer of the request (with ownership)
	virtual bool delegateRequest(
			char const* address,
			char const* method,
			char const* contentstr,
			size_t contentlen,
			WebRequestDelegateContextInterface* context)=0;

	/// \brief Get an answer structure for a simple message string that does not need a context to be defined
	/// \param[in] http_accept HTTP header variable 'Accept', a comma separated list of content types accepted by the client
	/// \param[in] html_base_href base link for HTML href links (base in HTML head)
	/// \param[in] name key used for the message in a markup language
	/// \param[in] message content string for the message
	/// \return answer answer structure (as error if an error occurred)
	virtual WebRequestAnswer getSimpleRequestAnswer(
			char const* http_accept,
			char const* html_base_href,
			char const* name,
			char const* messagestr,
			size_t messagelen)=0;
};

}//namespace
#endif


