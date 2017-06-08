/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _PAPUGA_CALL_RESULT_H_INCLUDED
#define _PAPUGA_CALL_RESULT_H_INCLUDED
/// \brief Representation of a result of a call to papuga language bindings
/// \file callResult.h
#include "papuga/typedefs.h"

#ifdef __cplusplus
extern "C" {
#endif
/// \brief Constructor of a CallResult
/// \param[out] self pointer to structure initialized by constructor
/// \param[in] errorbuf pointer to buffer to use for error messages
/// \param[in] errorbufsize allocation size of errorbuf in bytes
void papuga_init_CallResult( papuga_CallResult* self, char* errorbuf, size_t errorbufsize);

/// \brief Destructor of a CallResult
/// \param[in] self pointer to structure to free
void papuga_destroy_CallResult( papuga_CallResult* self);

/// \brief Define value of CallResult as int
/// \param[in,out] self pointer to structure
/// \param[in] val value to set as return value
void papuga_set_CallResult_int( papuga_CallResult* self, papuga_Int val);

/// \brief Define value of CallResult as unsigned int
/// \param[in,out] self pointer to structure
/// \param[in] val value to set as return value
void papuga_set_CallResult_uint( papuga_CallResult* self, papuga_UInt val);

/// \brief Define value of CallResult as double precision floating point value
/// \param[in,out] self pointer to structure
/// \param[in] val value to set as return value
void papuga_set_CallResult_double( papuga_CallResult* self, double val);

/// \brief Define value of CallResult as boolean value
/// \param[in,out] self pointer to structure
/// \param[in] val value to set as return value
void papuga_set_CallResult_bool( papuga_CallResult* self, bool val);

/// \brief Define value of CallResult as string (copied)
/// \param[in,out] self pointer to structure
/// \param[in] val value to set as return value
/// \return true on success, false on memory allocation error
bool papuga_set_CallResult_string( papuga_CallResult* self, const char* val, size_t valsize);

/// \brief Define value of CallResult as string reference
/// \param[in,out] self pointer to structure
/// \param[in] val value to set as return value
void papuga_set_CallResult_string_const( papuga_CallResult* self, const char* val, size_t valsize);

/// \brief Define value of CallResult as C-string (copied)
/// \param[in,out] self pointer to structure
/// \param[in] val value to set as return value
void papuga_set_CallResult_charp( papuga_CallResult* self, const char* val);

/// \brief Define value of CallResult as C-string reference
/// \param[in,out] self pointer to structure
/// \param[in] val value to set as return value
void papuga_set_CallResult_charp_const( papuga_CallResult* self, const char* val);

/// \brief Define value of CallResult as unicode string reference
/// \param[in,out] self pointer to structure
/// \param[in] enc character set encoding of the string
/// \param[in] val value to set as return value
void papuga_set_CallResult_langstring_const( papuga_CallResult* self, papuga_StringEncoding enc, const void* val, size_t valsize);

/// \brief Define value of CallResult as host object with ownership
/// \param[in,out] self pointer to structure
/// \param[in] classid class identifier of the host object
/// \param[in] data pointer to host object
/// \param[in] destroy delete function of the host object
void papuga_set_CallResult_hostobject( papuga_CallResult* self, int classid, void* data, papuga_Deleter destroy);

/// \brief Define value of CallResult as object of the binding language with ownership
/// \param[in,out] self pointer to structure
/// \param[in] val value to set as return value
void papuga_set_CallResult_langobject( papuga_CallResult* self, void* data, papuga_Deleter destroy);

/// \brief Define value of CallResult as serialization defined in the call result structure
/// \param[in,out] self pointer to structure
void papuga_set_CallResult_serialization( papuga_CallResult* self);

/// \brief Define value of CallResult as serialization defined in the call result structure with a host object reference with ownership
/// \param[in,out] self pointer to structure
/// \param[in] data pointer to host object
/// \param[in] destroy delete function of the host object
void papuga_set_CallResult_serialization_hostobject( papuga_CallResult* self, void* data, papuga_Deleter destroy);

/// \brief Define value of CallResult as iterator
/// \param[in,out] self pointer to structure
void papuga_set_CallResult_iterator( papuga_CallResult* self);

/// \brief Report an error of the call
/// \param[in,out] self pointer to structure
/// \param[in] msg format string of the error message
/// \param[in] ... arguments of the error message
void papuga_CallResult_reportError( papuga_CallResult* self, const char* msg, ...);

/// \brief Test if the call result has an error reported
/// \param[in,out] self pointer to structure
/// \return true, if yes
#define papuga_CallResult_hasError( self)		((self)->errorbuf.ptr[0] != 0)

/// \brief Get the error message of the call result
/// \param[in,out] self pointer to structure
/// \return the pointer to the message string
#define papuga_CallResult_lastError( self)		((self)->errorbuf.ptr)

#ifdef __cplusplus
}
#endif
#endif



