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

	/// \brief Create the structure for handling a request
	/// \param[in] schema identifier defining what type of request to execute
	/// \param[in] role role identifier of the request for checking permissions
	/// \param[out] status the status of the request
	/// \return the context structure for handling a request or NULL in case of an error
	virtual WebRequestContextInterface* createRequestContext(
			const char* schema,
			const char* role,
			WebRequestAnswer& status) const=0;
};

}//namespace
#endif


