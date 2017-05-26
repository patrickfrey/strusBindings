/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _PAPUGA_ERRORS_H_INCLUDED
#define _PAPUGA_ERRORS_H_INCLUDED
/// \brief Mapping of error codes to strings
/// \file errors.h

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Get an error message string for an error code
/// \param[in] errorcode error code
const char* papuga_ErrorCode_tostring( papuga_ErrorCode errorcode);

#define papuga_init_ErrorBuffer( buf, ptr_, size_)			{(buf)->ptr = (ptr_); (buf)->size = (size_); (buf)->ptr[0]=0;}

void papuga_ErrorBuffer_reportError( papuga_ErrorBuffer* self, const char* msg, ...);
#define papuga_ErrorBuffer_hasError( self)		((self)->ptr[0] != 0)
#define papuga_ErrorBuffer_lastError( self)		((self)->ptr)

#ifdef __cplusplus
}
#endif
#endif

