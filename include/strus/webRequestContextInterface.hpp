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
#include "strus/webRequestContent.hpp"
#include "strus/webRequestDelegateRequest.hpp"
#include <cstddef>
#include <vector>

namespace strus
{

/// \brief Interface for executing an XML/JSON request on the strus bindings
class WebRequestContextInterface
{
public:
	/// \brief Destructor
	virtual ~WebRequestContextInterface(){}

	/// \brief Run the request
	/// \param[in] method request method in uppercase
	/// \param[in] path path of the request
	/// \param[in] content content of the request
	/// \param[out] answer result status and the data of the answer of the request
	/// \param[out] delegateRequests delegate requests to perform for the completion of the request
	/// \return bool true if succeeded, false else
	virtual bool executeRequest(
			const char* method,
			const char* path,
			const WebRequestContent& content,
			WebRequestAnswer& answer,
			std::vector<WebRequestDelegateRequest>& delegateRequests)=0;

	/// \brief Execute a schema as partial request (e.g. return the result of a delegate request, finalize request)
	/// \param[in] schema schema to execute
	/// \param[in] content content of the result put
	/// \param[out] answer result status and the data of the answer of the operation
	/// \return bool true if succeeded, false else
	/// \remark Makes only sense after an executeRequest call that did not complete
	virtual bool executeSchemaPartialRequest(
			const char* schema,
			const WebRequestContent& content,
			WebRequestAnswer& answer)=0;

	/// \brief Create the answer structure for a simple message string
	/// \param[in] name key used for the message in a markup language
	/// \param[in] message content string for the message
	/// \param[out] answer result status and the data of the answer of the request
	/// \return bool true if succeeded, false else
	virtual bool getMessageAnswer(
			const std::string& name,
			const std::string& message,
			WebRequestAnswer& answer)=0;

	/// \brief Load the main configuration initializing the root context object
	/// \param[in] content content of the configuration to load
	/// \param[out] answer result status
	/// \return bool true if succeeded, false else
	virtual bool executeLoadMainConfiguration(
			const WebRequestContent& content,
			WebRequestAnswer& answer)=0;

	/// \brief Load a sub configuration initializing an object addressable by type and name
	/// \param[in] typenam type name of the object
	/// \param[in] contextnam name of the object
	/// \param[in] content content of the configuration to load
	/// \param[out] answer result status
	/// \param[out] delegateRequests delegate requests to perform for the completion of the request
	/// \return bool true if succeeded, false else
	virtual bool executeLoadSubConfiguration(
			const char* typenam,
			const char* contextnam,
			const WebRequestContent& content,
			WebRequestAnswer& answer,
			std::vector<WebRequestDelegateRequest>& delegateRequests)=0;
};

}//namespace
#endif


