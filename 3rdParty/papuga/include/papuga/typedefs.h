/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _PAPUGA_TYPEDEFS_H_INCLUDED
#define _PAPUGA_TYPEDEFS_H_INCLUDED
/// \brief Typedefs of papuga data structures
/// \file typedefs.h
#include <stddef.h>

#ifdef _MSC_VER
#error stdint definitions missing for Windows
#else
#include <stdint.h>
#endif
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Error codes for conversion functions
typedef enum papuga_ErrorCode
{
	papuga_Ok=0,
	papuga_LogicError=1,
	papuga_NoMemError=2,
	papuga_TypeError=3,
	papuga_EncodingError=4,
	papuga_BufferOverflowError=5,
	papuga_OutOfRangeError=6
} papuga_ErrorCode;

/// \brief Object of the host environment
typedef void papuga_HostObjectType;

/// \brief Enumeration of value type identifiers
typedef enum papuga_Type {
	papuga_Void			= 0x00,
	papuga_Double			= 0x01,
	papuga_UInt			= 0x02,
	papuga_Int			= 0x03,
	papuga_String			= 0x04,
	papuga_LangString		= 0x05,
	papuga_HostObject		= 0x06,
	papuga_Serialized		= 0x07
} papuga_Type;

/// \brief Unsigned integer type as represented by papuga
typedef uint64_t papuga_UIntType;
/// \brief Signed integer type as represented by papuga
typedef int64_t papuga_IntType;
/// \brief Floating point value type as represented by papuga
typedef double papuga_FloatType;

typedef enum papuga_StringEncoding {
	papuga_UTF8,		///< Unicode UTF-8 encoding
	papuga_UTF16BE,		///< Unicode UTF-16 big endian encoding
	papuga_UTF16LE,		///< Unicode UTF-16 little endian encoding
	papuga_UTF16,		///< Unicode UTF-16 machine endianess encoding
	papuga_UTF32BE,		///< Unicode chars big endian
	papuga_UTF32LE,		///< Unicode chars little endian
	papuga_UTF32		///< Unicode chars in machine endianess encoding
} papuga_StringEncoding;

/// \brief Node type tag of a papuga serialization node
typedef enum papuga_Tag
{
	papuga_TagOpen,		///< Open a scope
	papuga_TagClose,	///< Closes a scope
	papuga_TagName,		///< The name of the following value (Value) or structure (Open)
	papuga_TagValue		///< Atomic value
} papuga_Tag;

typedef struct papuga_Serialization* papuga_SerializationPtr;

/// \brief Representation of a variadic value type
typedef struct papuga_ValueVariant
{
	int valuetype:8;					///< casts to a papuga_Type
	int encoding:8;						///< casts to a papuga_StringEncoding
	int classid:16;						///< casts to a class index (starting from 1) pointing to the class method table in the scripting language as defined in the language description
	int32_t length;						///< length of the string or serialization in items (UTF-8 item length = 1, UTF-16 item length = 2, etc.)
	union {
		double Double;					///< double precision floating point value
		uint64_t UInt;					///< unsigned integer value
		int64_t Int;					///< signed integer value
		const char* string;				///< null terminated UTF-8 string (host string representation)
		const void* langstring;				///< string value (not nessesarily null terminated) for other character set encodings (binding language string representation)
		const papuga_HostObjectType* hostObject;	///< object represented in the host environment
		papuga_SerializationPtr serialization;		///< object serialization reference
	} value;
} papuga_ValueVariant;

/// \brief One node of a papuga serialization
typedef struct papuga_Node
{
	papuga_Tag tag;
	papuga_ValueVariant value;
} papuga_Node;

/// \brief Papuga serialization structure
typedef struct papuga_Serialization
{
	unsigned int allocsize;					///< allocation size of the array
	unsigned int arsize;					///< number of nodes
	papuga_Node* ar;					///< array of nodes
} papuga_Serialization;

/// \brief Destructor function of a host object
typedef void (*papuga_HostObjectDeleter)( void* obj);

/// \brief Papuga host object reference
typedef struct papuga_HostObjectReference
{
	void* data;
	papuga_HostObjectDeleter destroy;
} papuga_HostObjectReference;

typedef struct papuga_StringBuffer
{
	unsigned int allocsize;			///< allocation size of the string
	unsigned int arsize;			///< number of characters without 0-termination
	char* ar;				///< pointer to allocated string
	struct papuga_StringBuffer* next;	///< next buffer in linked list of buffers
} papuga_StringBuffer;

typedef struct papuga_CallResult
{
	papuga_ValueVariant value;
	papuga_HostObjectReference object;
	papuga_Serialization serialization;
	papuga_StringBuffer valuebuf;
	char errorbuf[ 2048];
} papuga_CallResult;

#ifdef __cplusplus
}
#endif
#endif

