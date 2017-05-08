/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
/// \brief Map of papuga error codes to std::runtime_error exception
/// \file papugaErrorException.hpp
#include "papugaErrorException.hpp"
#include "internationalization.hpp"

using namespace strus;
using namespace strus::bindings;

std::runtime_error bindings::papuga_error_exception( const papuga_ErrorCode& ec, const char* where)
{
	switch (ec)
	{
		case papuga_Ok: break;
		case papuga_LogicError: return strus::runtime_error(_TXT("internal: logic error in %s"), where);
		case papuga_NoMemError: return strus::runtime_error(_TXT("out of memory in %s"), where);
		case papuga_TypeError: return strus::runtime_error(_TXT("conversion error in %s"), where);
		case papuga_EncodingError: return strus::runtime_error(_TXT("string encoding error in %s"), where);
		case papuga_BufferOverflowError: return strus::runtime_error(_TXT("buffer owerflow error in %s"), where);
		case papuga_OutOfRangeError: return strus::runtime_error(_TXT("argument out of range in %s"), where);
		case papuga_NofArgsError: return strus::runtime_error(_TXT("number of arguments do not match in %s"), where);
		case papuga_MissingSelf: return strus::runtime_error(_TXT("illegal callof method without self reference in %s"), where);
	}
	throw strus::runtime_error(_TXT("internal: exception without known cause in %s"), where);
}

