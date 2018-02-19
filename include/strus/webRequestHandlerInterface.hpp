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

/// \brief Interface for executing XML/JSON requests based on the strus bindings API
class WebRequestHandlerInterface
{
public:
	/// \brief Destructor
	virtual ~WebRequestHandlerInterface(){}

	/// \brief Create the structure for handling a request
	/// \param[in] context identifier defining where to execute the request
	/// \param[in] schema identifier defining what type of request to execute (without namespace prefix)
	/// \param[in] accepted_charset HTTP header variable 'Accept-Charset', a comma separated list of character set encodings accepted by the client
	/// \param[out] answer the error status
	/// \return the context structure for handling a request or NULL in case of an error (inspect answer for the error details)
	virtual WebRequestContextInterface* createContext(
			const char* context,
			const char* schema,
			const char* accepted_charset,
			const char* accepted_doctype,
			WebRequestAnswer& answer) const=0;

	/// \brief Test if a schema exists
	/// \param[in] schema identifier queried
	/// \return true, if a schema with this name exists (including namespace prefix)
	virtual bool hasSchema( const char* context, const char* schema) const=0;

	/// \brief Execute a list command for introspection defined by a path (URL)
	virtual bool executeList(
			const std::vector<std::string>& path,
			std::vector<std::string>& result,
			WebRequestAnswer& answer) const=0;

	/// \brief Execute a view command for introspection defined by a path (URL)
	virtual bool executeView(
			const std::vector<std::string>& path,
			const char* accepted_charset,
			const char* accepted_doctype,
			WebRequestAnswer& answer) const=0;

	/// \brief Execute a configuration request
	/// \param[in] destContextType type of the created context
	/// \param[in] destContextName identifier of the created context
	/// \param[in] srcContextName identifier defining the base context (NULL for none)
	/// \param[in] schema identifier defining what type of request to execute for loading the configuration
	/// \param[in] content content of the configuration to load
	/// \param[out] answer the error status
	/// \return bool in case of success, false in case of an error (inspect answer for the error details)
	virtual bool loadConfiguration(
			const char* destContextType,
			const char* destContextName,
			const char* srcContextName,
			const char* schema,
			const WebRequestContent& content,
			WebRequestAnswer& answer)=0;
};

}//namespace
#endif


