/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Library for handling XML/JSON requests
/// \file webrequest.hpp
#ifndef _STRUS_WEBREQUEST_LIB_HPP_INCLUDED
#define _STRUS_WEBREQUEST_LIB_HPP_INCLUDED

/// \brief strus toplevel namespace
namespace strus {

/// \brief Forward declaration
class WebRequestHandlerInterface;
/// \brief Forward declaration
class WebRequestLoggerInterface;

WebRequestHandlerInterface* createWebRequestHandler( WebRequestLoggerInterface* logger);

}//namespace
#endif

