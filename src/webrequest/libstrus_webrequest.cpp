/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Library for handling XML/JSON requests
/// \file libstrus_webrequest.cpp
#include "strus/lib/webrequest.hpp"
#include "webRequestHandler.hpp"
#include "private/internationalization.hpp"
#include <new>

/// \brief strus toplevel namespace
using namespace strus;

WebRequestHandlerInterface* strus::createWebRequestHandler( WebRequestLoggerInterface* logger)
{
	return new (std::nothrow) WebRequestHandler( logger);
}



