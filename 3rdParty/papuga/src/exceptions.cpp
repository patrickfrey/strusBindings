/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
/// \brief Map of papuga error codes to std::runtime_error exception
/// \file exceptions.hpp
#include "papuga/exceptions.hpp"
#include <cstdio>

/// \note Internationalization support still missing
#define _TXT(str) str

using namespace papuga;

std::runtime_error papuga::error_exception( const papuga_ErrorCode& ec, const char* where)
{
	char buf[ 1024];
	switch (ec)
	{
		case papuga_Ok: break;
		case papuga_LogicError: snprintf( buf, sizeof(buf), _TXT("internal: logic error in %s"), where); break;
		case papuga_NoMemError: snprintf( buf, sizeof(buf), _TXT("out of memory in %s"), where); break;
		case papuga_TypeError: snprintf( buf, sizeof(buf), _TXT("conversion error in %s"), where); break;
		case papuga_EncodingError: snprintf( buf, sizeof(buf), _TXT("string encoding error in %s"), where); break;
		case papuga_BufferOverflowError: snprintf( buf, sizeof(buf), _TXT("buffer owerflow error in %s"), where); break;
		case papuga_OutOfRangeError: snprintf( buf, sizeof(buf), _TXT("argument out of range in %s"), where); break;
		case papuga_NofArgsError: snprintf( buf, sizeof(buf), _TXT("number of arguments do not match in %s"), where); break;
		case papuga_MissingSelf: snprintf( buf, sizeof(buf), _TXT("illegal call of method without self reference in %s"), where); break;
		case papuga_InvalidAccess: snprintf( buf, sizeof(buf), _TXT("invalid access (illegal pointer) in %s"), where); break;
		case papuga_UnexpectedEof: snprintf( buf, sizeof(buf), _TXT("unexpected end of serialization in %s"), where); break;
		default: snprintf( buf, sizeof(buf), _TXT( "internal: exception without known cause in %s"), where); break;
	}
	buf[ sizeof(buf)-1] = 0;
	return std::runtime_error( buf);
}


