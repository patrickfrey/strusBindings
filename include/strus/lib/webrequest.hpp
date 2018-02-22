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
#include "strus/webRequestContent.hpp"
#include <cstddef>

/// \brief strus toplevel namespace
namespace strus {

/// \brief Forward declaration
class WebRequestHandlerInterface;
/// \brief Forward declaration
class WebRequestLoggerInterface;

/// \brief Create a web request handler
/// \param[in] request logger interface
/// \param[in] html_head content included in HTML <head> section when returning HTML
/// \return pointer to handler in case of success, NULL in case of memory allocation error
WebRequestHandlerInterface* createWebRequestHandler( WebRequestLoggerInterface* logger, const std::string& html_head);

/// \brief Convert string from UTF-8 to a given charset encoding
/// \param[in] charset character set encoding to convert to
/// \param[in] destbuf where to write result to
/// \param[in] destbufsize size of buffer in bytes where to write result to
/// \param[in] length length of string returned
/// \param[in] src pointer to string to convert
/// \param[in] srcsize number of bytes in src
/// \return pointer to result string or NULL in case of error
/// \note The error handling of this function is poor. Should only ne used for error or status messages, where any error can be assumed as fatal
const char* convertContentCharset( const char* charset, char* destbuf, std::size_t destbufsize,  std::size_t& length, const char* src, std::size_t srcsize);

/// \brief Select the best choice for the character set for messages not related to an answer of a request
/// \param[in] http_accept_charset list of character sets accepted taken from the HTTP header
/// \return best choice of a character set
const char* selectAcceptedCharset( const char* http_accept_charset);

/// \brief Select the best choice for the content type for messages not related to an answer of a request
/// \param[in] http_accept list of content types accepted taken from the HTTP header
/// \return best choice of a content type
WebRequestContent::Type selectAcceptedContentType( const char* http_accept);

}//namespace
#endif

