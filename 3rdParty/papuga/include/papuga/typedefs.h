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
	papuga_OutOfRangeError=6,
	papuga_NofArgsError=7,
	papuga_MissingSelf=8,
	papuga_InvalidAccess=9,
	papuga_UnexpectedEof=10,
	papuga_NotImplemented=11
} papuga_ErrorCode;

/// \brief Static buffer for error message
typedef struct papuga_ErrorBuffer
{
	char* ptr;
	size_t size;
} papuga_ErrorBuffer;

/// \brief Object of the host environment
typedef struct papuga_HostObject papuga_HostObject;
/// \brief Call result structure
typedef struct papuga_CallResult papuga_CallResult;

/// \brief Enumeration of value type identifiers
typedef enum papuga_Type {
	papuga_TypeVoid			= 0x00,		///< NULL Value without type
	papuga_TypeDouble		= 0x01,		///< Double precision floating point value (C double)
	papuga_TypeUInt			= 0x02,		///< Unsigned integer value (maximum width 64 bits)
	papuga_TypeInt			= 0x03,		///< Signed integer value (maximum width 64 bits)
	papuga_TypeString		= 0x04,		///< Host environment string (null-terminated UTF-8)
	papuga_TypeLangString		= 0x05,		///< Bindings language string (unicode string with a defined encoding - papuga_StringEncoding)
	papuga_TypeHostObject		= 0x06,		///< Class object defined in the host environment, part of the interface
	papuga_TypeSerialization	= 0x07,		///< Serialization of an object constructed in the binding language
	papuga_TypeIterator		= 0x08		///< Iterator closure
} papuga_Type;

/// \brief Unsigned integer type as represented by papuga
typedef uint64_t papuga_UInt;
/// \brief Signed integer type as represented by papuga
typedef int64_t papuga_Int;
/// \brief Floating point value type as represented by papuga
typedef double papuga_Float;

/// \brief Enumeration of character set encodings used for strings defined in the binding language (papuga_LangString)
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

typedef struct papuga_Serialization papuga_Serialization;
typedef struct papuga_Iterator papuga_Iterator;

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
		const void* hostObjectData;			///< reference to the data of an object represented in the host environment
		papuga_Serialization* serialization;		///< reference of an object serialization 
		papuga_Iterator* iterator;			///< reference of an iterator closure
	} value;
} papuga_ValueVariant;

/// \brief One node of a papuga serialization
typedef struct papuga_Node
{
	papuga_Tag tag;				///< tag of the serialization node
	papuga_ValueVariant value;		///< value of the serialization node
} papuga_Node;

/// \brief Papuga serialization structure
struct papuga_Serialization
{
	unsigned int allocsize;			///< allocation size of the array
	unsigned int arsize;			///< number of nodes
	papuga_Node* ar;			///< array of nodes
};

/// \brief Destructor function of an object
typedef void (*papuga_Deleter)( void* obj);

/// \brief Papuga host object 
struct papuga_HostObject
{
	void* data;				///< pointer to the object
	papuga_Deleter destroy;			///< destructor of the host object in case of this structure holding ownership of it
};

/// \brief Get next method of an iterator
typedef bool (*papuga_GetNext)( void* self, papuga_CallResult* result);

/// \brief Papuga iterator closure 
struct papuga_Iterator
{
	void* data;				///< pointer to the object
	papuga_Deleter destroy;			///< destructor of the iterated object in case of this structure holding ownership of it
	papuga_GetNext getNext;			///< method to fetch the next iteration element
};

/// \brief Buffer for strings copied
typedef struct papuga_Allocator
{
	unsigned int allocsize;			///< allocation size of this block
	unsigned int arsize;			///< number of bytes allocated in this block
	char* ar;				///< pointer to memory
	struct papuga_Allocator* next;		///< next buffer in linked list of buffers
} papuga_Allocator;

/// \brief Structure representing the result of an interface method call
struct papuga_CallResult
{
	papuga_ValueVariant value;		///< result value
	papuga_HostObject object;		///< reference (with or without ownership) to an object that lives in the host environment
	papuga_Serialization serialization;	///< serialization of the result in case of a structure
	papuga_Iterator iterator;		///< iterator closure object
	papuga_Allocator allocator;		///< allocator for values that had to be copied
	papuga_ErrorBuffer errorbuf;		///< static buffer for error message
};

#ifdef __cplusplus
}
#endif
#endif

