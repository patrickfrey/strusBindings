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
	/// \param[in] accepted_charset HTTP header variable 'Accept-Charset', a comma separated list of character set encodings accepted by the client
	/// \param[in] accepted_doctype HTTP header variable 'Accept', a comma separated list of content types accepted by the client
	/// \param[out] answer the error status
	/// \return the context structure for handling a request or NULL in case of an error (inspect answer for the error details)
	virtual WebRequestContextInterface* createContext(
			const char* accepted_charset,
			const char* accepted_doctype,
			WebRequestAnswer& answer) const=0;

	/// \brief Test if a scheme exists
	/// \param[in] contextType identifier defining the type of the context for the scheme queried
	/// \param[in] scheme identifier queried
	/// \return true, if a scheme with this name exists
	virtual bool hasScheme(
			const char* contextType,
			const char* scheme) const=0;

	/// \brief Get the list of available schemes
	/// \return the list of available schemes
	virtual char const** schemes() const=0;

	/// \brief Execute a configuration request
	/// \param[in] contextType type of the created context
	/// \param[in] contextName identifier of the created context
	/// \param[in] scheme identifier defining what type of request to execute for loading the configuration
	/// \param[in] content content of the configuration to load
	/// \param[out] answer the error status
	/// \return bool in case of success, false in case of an error (inspect answer for the error details)
	/// \note the parent context is implicitely derived from the internal scheme dependency definitions
	virtual bool loadConfiguration(
			const char* contextType,
			const char* contextName,
			const char* scheme,
			const WebRequestContent& content,
			WebRequestAnswer& answer)=0;

	/// \brief Store configuration to be reloaded with loadStoredConfigurations()
	/// \param[in] contextType type of the created context
	/// \param[in] contextName identifier of the created context
	/// \param[in] scheme identifier defining what type of request to execute for loading the configuration
	/// \param[in] content content of the configuration to load
	/// \param[out] answer the error status
	/// \return bool in case of success, false in case of an error (inspect answer for the error details)
	virtual bool storeConfiguration(
			const char* contextType,
			const char* contextName,
			const char* scheme,
			const WebRequestContent& content,
			WebRequestAnswer& answer) const=0;

	/// \brief Reload configurations stored with storedConfiguration()
	/// \param[out] answer the error status
	/// \return bool in case of success, false in case of an error (inspect answer for the error details)
	virtual bool loadStoredConfigurations(
			WebRequestAnswer& answer)=0;
};

}//namespace
#endif


