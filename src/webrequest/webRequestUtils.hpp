/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Helper functions and classes for executing XML/JSON requests on the strus bindings
/// \file "webRequestUtils.hpp"
#ifndef _STRUS_WEB_REQUEST_UTILS_HPP_INCLUDED
#define _STRUS_WEB_REQUEST_UTILS_HPP_INCLUDED
#include "papuga/typedefs.h"
#include "papuga/requestParser.h"
#include "strus/errorCodes.hpp"
#include "strus/webRequestContent.hpp"
#include <cstddef>
#include <string>

namespace strus {

ErrorCause papugaErrorToErrorCause( papuga_ErrorCode errcode);
int errorCauseToHttpStatus( ErrorCause cause);

std::string webRequestContent_tostring( const WebRequestContent& content);

papuga_StringEncoding getResultStringEncoding( const char* accepted_charset, papuga_StringEncoding inputenc);
papuga_ContentType getResultContentType( const char* http_accept, papuga_ContentType inputdoctype);

}//namespace
#endif


