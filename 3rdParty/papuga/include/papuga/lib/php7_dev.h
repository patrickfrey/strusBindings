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

/// \brief Map of class identifiers to zend_class_entry structures (for creating objects with classid as only info)
typedef struct papuga_php_ClassEntryMap
{
	size_t size;				///< size of map in elements
	papuga_zend_class_entry** ar;		///< pointer to PHP Zend class entry structures
} papuga_php_ClassEntryMap;

/// \brief Initialize papuga globals for PHP
/// \remark this function has to be called before using any of the functions of this module
void papuga_php_init();

/// \brief Create a non initialized (NULL) host object in the PHP environment
/// \param[in] ce class description (method table for PHP)
papuga_zend_object* papuga_php_create_object( papuga_zend_class_entry* ce);

/// \brief Initializes a zend object created with papuga_php_create_object with its host object reference
/// \param[in] selfzval zval of the object to initialize
/// \param[in] hostobjref pointer to host object reference structure (passed with ownership, freed on error)
/// \return true in success, error on a type mismatch error
bool papuga_php_init_object( void* selfzval, papuga_HostObject* hostobjref);

/// \brief Fills a structure with the arguments passed in a PHP binding function/method call
/// \param[in] selfzval the zend zval pointer of the self parameter of the object called
/// \param[in] argc number of function arguments
/// \param[out] arg argument structure initialized
bool papuga_php_init_CallArgs( void* selfzval, int argc, papuga_php_CallArgs* arg);

/// \brief Frees the arguments of a papuga call (to call after the call)
/// \param[in] arg argument structure freed
void papuga_php_destroy_CallArgs( papuga_php_CallArgs* arg);

/// \brief Transfers the call result of a binding function into the PHP context, freeing the call result structure
/// \param[out] zval_return_value zend variable (zval*) where to write the call result to
/// \param[in,out] retval return values to move to PHP context
/// \param[in] cemap map of class ids to zend class descriptions
/// \param[in,out] errbuf buffer for error messages
/// \return true on success, false on failure
bool papuga_php_move_CallResult( void* zval_return_value, papuga_CallResult* retval, const papuga_php_ClassEntryMap* cemap, papuga_ErrorBuffer* errbuf);

#endif

