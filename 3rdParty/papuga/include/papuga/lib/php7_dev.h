/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
/// \brief Library interface for PHP (v7) bindings built by papuga
/// \file papuga/lib/php7_dev.h
#ifndef _PAPUGA_PHP_DEV_LIB_H_INCLUDED
#define _PAPUGA_PHP_DEV_LIB_H_INCLUDED
#include "papuga/typedefs.h"
#include "papuga/allocator.h"

#define papuga_PHP_MAX_NOF_ARGUMENTS 64

typedef struct papuga_php_CallArgs
{
	int erridx;
	papuga_ErrorCode errcode;
	size_t argc;
	void* self;
	papuga_Allocator allocator;
	papuga_ValueVariant argv[ papuga_PHP_MAX_NOF_ARGUMENTS];
	int allocbuf[ 1024];
} papuga_php_CallArgs;

/// \brief Handles for PHP internal structures
/// \remark Tunnel them (explicit reinterpret cast on both sides) to avoid poisoned includes
typedef void papuga_zend_object;	//... struct _zend_object = zend_object declared in zend.h
typedef void papuga_zend_class_entry;	//... struct _zend_class_entry = zend_class_entry declared in zend.h

/// \brief Initialize papuga globals for PHP
/// \remark this function has to be called before using any of the functions of this module
void papuga_php_init();

/// \brief Create a host object in the PHP environment
/// \param[in] ce class description (method table for PHP)
/// \param[in] hostobj to object representation in the host environment
papuga_zend_object* papuga_php_create_object(
	papuga_zend_class_entry* ce,
	const papuga_HostObject* hostobj);

/// \brief Fills a structure with the arguments passed in a PHP binding function/method call
/// \param[in] selfzval the zend zval pointer of the self parameter of the object called
/// \param[in] argc number of function arguments
/// \param[out] arg argument structure initialized
bool papuga_php_init_CallArgs( void* selfzval, int argc, papuga_php_CallArgs* arg);

/// \brief Frees the arguments of a papuga call (to call after the call)
/// \param[in] arg argument structure freed
void papuga_php_destroy_CallArgs( papuga_php_CallArgs* arg);

/// \brief Transfers the call result of a binding function into the PHP context, freeing the call result structure
/// \param[in,out] retval return values to move to PHP context
void papuga_php_move_CallResult( papuga_CallResult* retval);

/// \brief Raise a PHP error
/// \param[in] fmt format string of the error message
/// \param[in] ... format arguments of the error message
void papuga_php_error( const char* fmt, ...);

#endif

