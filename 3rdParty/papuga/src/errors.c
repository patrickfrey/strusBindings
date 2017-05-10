/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Representation of a result of a call to papuga language bindings
/// \file callResult.c
#include "papuga/typedefs.h"

/// \brief Hook for GETTEXT
#define _TXT(x) x

const char* papuga_ErrorCode_tostring( int errorcode)
{
	switch (errorcode)
	{
		case papuga_Ok: return _TXT("Ok");
		case papuga_LogicError: return _TXT("logic error");
		case papuga_NoMemError: return _TXT("out of memory");
		case papuga_TypeError: return _TXT("type mismatch");
		case papuga_EncodingError: return _TXT("string character encoding error");
		case papuga_BufferOverflowError: return _TXT("internal buffer not big enough");
		case papuga_OutOfRangeError: return _TXT("value out of range");
		case papuga_NofArgsError: return _TXT("number of arguments does not match");
		case papuga_MissingSelf: return _TXT("self argument is missing");
		default: return _TXT("unknown error");
	}
}

