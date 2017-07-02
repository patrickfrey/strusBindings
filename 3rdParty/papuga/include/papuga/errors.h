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
#include "papuga/typedefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Get an error message string for an error code
/// \param[in] errorcode error code
const char* papuga_ErrorCode_tostring( papuga_ErrorCode errorcode);

/// \brief Constructor of ErrorBuffer
/// \param[out] self pointer to structure 
/// \param[in] ptr_ pointer to buffer
/// \param[in] size_ allocation size of the buffer in bytes
#define papuga_init_ErrorBuffer( self, ptr_, size_)			{papuga_ErrorBuffer* s = self; s->ptr = (ptr_); s->size = (size_); if (s->size) s->ptr[0]=0;}

/// \brief Storing the current error in the buffer
/// \param[in,out] self pointer to structure
/// \param[in] msg format string of the error message
/// \param[in] ... arguments of the error message
void papuga_ErrorBuffer_reportError( papuga_ErrorBuffer* self, const char* msg, ...);

/// \brief Test if the error buffer has an error reported
/// \param[in] self pointer to structure
#define papuga_ErrorBuffer_hasError( self)		((self)->ptr[0] != 0)

/// \brief Get the pointer to the error message string
/// \param[in] self pointer to structure
#define papuga_ErrorBuffer_lastError( self)		((self)->ptr)

#ifdef __cplusplus
}
#endif
#endif

