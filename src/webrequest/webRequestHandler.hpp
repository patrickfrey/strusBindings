/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Implementation of the interface for executing XML/JSON requests on the strus bindings
/// \file "webRequestHandler.hpp"
#ifndef _STRUS_WEB_REQUEST_HANDLER_IMPL_HPP_INCLUDED
#define _STRUS_WEB_REQUEST_HANDLER_IMPL_HPP_INCLUDED
#include "strus/webRequestHandlerInterface.hpp"
#include "papuga/requestHandler.h"
#include <cstddef>

namespace strus
{

/// \brief Implementation of the interface for executing XML/JSON requests on the strus bindings
class WebRequestHandler
	:public WebRequestHandlerInterface
{
public:
	WebRequestHandler();
	virtual ~WebRequestHandler();

	virtual WebRequestContextInterface* createRequestContext(
			const char* schema,
			const char* role,
			WebRequestAnswer& status) const;

private:
	papuga_RequestHandler* m_impl;
};

}//namespace
#endif


