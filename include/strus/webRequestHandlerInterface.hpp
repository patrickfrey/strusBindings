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

namespace strus
{

/// \brief Forward declaration
class WebRequestContextInterface;

/// \brief Interface for executing XML/JSON requests based on the strus bindings API
class WebRequestHandlerInterface
{
public:
	/// \brief Destructor
	virtual ~WebRequestHandlerInterface(){}

	/// \brief Create the structure for handling a request
	/// \param[in] context identifier defining where to execute the request
	/// \param[in] schema identifier defining what type of request to execute (without namespace prefix)
	/// \param[in] role role identifier of the request for checking permissions
	/// \param[in] accepted_charset HTTP header variable 'Accept-Charset', a comma separated list of character set encodings accepted by the client
	/// \param[out] answer the error status
	/// \return the context structure for handling a request or NULL in case of an error (inspect answer for the error details)
	virtual WebRequestContextInterface* createContext(
			const char* context,
			const char* schema,
			const char* role,
			const char* accepted_charset,
			WebRequestAnswer& answer) const=0;

	/// \brief Test if a schema exists
	/// \param[in] schema identifier queried
	/// \return true, if a schema with this name exists (including namespace prefix)
	virtual bool hasSchema( const char* schema) const=0;

	/// \brief Execute a configuration request
	/// \param[in] destContextName identifier defining the name of the context created as result of loading the configuration
	/// \param[in] destContextSchemaPrefix schemaprefix assigned to the created context
	/// \param[in] srcContextName identifier defining the base context (NULL for none)
	/// \param[in] schema identifier defining what type of request to execute for loading the configuration
	/// \param[in] content content of the configuration to load
	/// \param[out] answer the error status
	/// \return bool in case of success, false in case of an error (inspect answer for the error details)
	virtual bool loadConfiguration(
			const char* destContextName,
			const char* destContextSchemaPrefix,
			const char* srcContextName,
			const char* schema,
			const WebRequestContent& content,
			WebRequestAnswer& answer)=0;
};

}//namespace
#endif


