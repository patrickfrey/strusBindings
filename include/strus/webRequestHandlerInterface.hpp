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

	/// \brief Start background thread (for garbage collector or similar stuff)
	/// \note Has to be called after constructor in the main loop if serving requests
	virtual bool start()=0;

	/// \brief Stop background thread (for garbage collector or similar stuff)
	/// \note Has to be called after constructor in the main loop if serving requests
	virtual void stop()=0;

	/// \brief Create the structure for handling a request
	/// \param[in] accepted_charset HTTP header variable 'Accept-Charset', a comma separated list of character set encodings accepted by the client
	/// \param[in] accepted_doctype HTTP header variable 'Accept', a comma separated list of content types accepted by the client
	/// \param[in] html_base_href base link for HTML href links (base in HTML head)
	/// \param[out] answer the error status
	/// \return the context structure for handling a request or NULL in case of an error (inspect answer for the error details)
	virtual WebRequestContextInterface* createContext(
			const char* accepted_charset,
			const char* accepted_doctype,
			const char* html_base_href,
			WebRequestAnswer& answer)=0;
};

}//namespace
#endif


