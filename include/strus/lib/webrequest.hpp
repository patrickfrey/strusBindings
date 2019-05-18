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
/// \brief Forward declaration
class ErrorBufferInterface;

/// \brief Create a web request handler
/// \param[in] request logger interface (no ownership)
/// \param[in] html_head content included in HTML <head> section when returning HTML
/// \param[in] config_store_dir directory where to store configurations loaded with PUT
/// \param[in] config main configuration
/// \param[in] maxIdleTime maximum time of keepalive for untouched transactions in seconds
/// \param[in] maxDelegateTotalConn max simultaneously open connections to delegate sub requests
/// \param[in] maxDelegateHostConn set max number of simultaneous delegate request connections to a single host
/// \param[in] nofTransactionsPerSeconds 2nd allocation dimension value for the sliding window used internally for open transactions besides maxIdleTime
/// \param[in] errorhnd error buffer interface to use
/// \return pointer to handler in case of success, NULL in case of memory allocation error
WebRequestHandlerInterface* createWebRequestHandler(
		WebRequestLoggerInterface* logger,
		const std::string& html_head,
		const std::string& config_store_dir,
		const std::string& config,
		int maxIdleTime,
		int maxDelegateTotalConn,
		int maxDelegateHostConn,
		int nofTransactionsPerSeconds,
		ErrorBufferInterface* errorhnd);

/// \brief Store all request schemas of a given type of the web request handler provided here to a directory
/// \param[in] config main configuration
/// \param[in] dir absolute path where to write the descriptions to (created if it does not exist)
/// \param[in] doctype type of the schema ("xml"/"xsd" for XML Schema and "json" for JSON Schema)
/// \param[in] errorhnd error buffer interface to use
/// \return true in case of success, false else
bool storeWebRequestSchemaDescriptions(
		const std::string& config,
		const std::string& dir,
		const std::string& doctype,
		ErrorBufferInterface* errorhnd);

/// \brief Convert string from UTF-8 to a given charset encoding
/// \param[in] charset character set encoding to convert to
/// \param[in] destbuf where to write result to
/// \param[in] destbufsize size of buffer in bytes where to write result to
/// \param[in] length length of string returned
/// \param[in] src pointer to string to convert
/// \param[in] srcsize number of bytes in src
/// \return pointer to result string or NULL in case of error
/// \note The error handling of this function is poor. Should only ne used for error or status messages, where any error can be assumed as fatal
const char* convertContentCharset(
		const char* charset,
		char* destbuf,
		std::size_t destbufsize,
		std::size_t& length,
		const char* src,
		std::size_t srcsize);

/// \brief Select the best choice for the character set for messages not related to an answer of a request
/// \param[in] http_accept_charset list of character sets accepted taken from the HTTP header
/// \return best choice of a character set
const char* selectAcceptedCharset( const char* http_accept_charset);

/// \brief Select the best choice for the content type for messages not related to an answer of a request
/// \param[in] http_accept list of content types accepted taken from the HTTP header
/// \return best choice of a content type
WebRequestContent::Type selectAcceptedContentType( const char* http_accept);

/// \brief Guess the type of the document (without encoding) as string, e.g. "application/json"
/// \param[in] content pointer to the content
/// \param[in] contentsize number of bytes in content
/// \return the mime type of the document as string or NULL if unknown
const char* guessContentType( const char* content, std::size_t contentsize);

/// \brief Guess the character set encoding of a document as string, e.g. "UTF-16BE"
/// \param[in] content pointer to the content
/// \param[in] contentsize number of bytes in content
/// \return the encoding as string or NULL if unknown
const char* guessCharsetEncoding( const char* content, std::size_t contentsize);

}//namespace
#endif

