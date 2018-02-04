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

	/// \brief Test if a schema exists
	/// \param[in] schema identifier queried
	/// \return true, if a schema with this name exists
	virtual bool hasSchema( const char* schema) const=0;

	/// \brief Create the structure for handling a request
	/// \param[in] context identifier defining where to execute the request
	/// \param[in] schema identifier defining what type of request to execute
	/// \param[in] role role identifier of the request for checking permissions
	/// \param[out] status the status of the request
	/// \return the context structure for handling a request or NULL in case of an error
	virtual WebRequestContextInterface* createContext(
			const char* context,
			const char* schema,
			const char* role,
			WebRequestAnswer& status) const=0;

	/// \brief Execute a configuration request
	/// \param[in] destContext identifier defining the context created as execution result
	/// \param[in] srcContext identifier defining where to execute the request (NULL for root context)
	/// \param[in] schema identifier defining what type of request to execute
	/// \param[in] doctype document content type "XML" (or "application/xml") or "JSON" (or "application/xml") 
	/// \param[in] encoding character set encoding, e.g. "UTF-8' or "UTF-16"  or "UTF-16BE"
	/// \param[in] contentstr pointer to content of the request
	/// \param[in] contentlen size of content in bytes
	/// \param[out] status the status of the request
	/// \return bool in case of success, false in case of error (inspect status for the error)
	virtual bool executeConfiguration(
			const char* destContext,
			const char* srcContext,
			const char* schema,
			const char* doctype,
			const char* encoding, 
			const char* contentstr,
			std::size_t contentlen,
			WebRequestAnswer& status) const=0;
};

}//namespace
#endif


