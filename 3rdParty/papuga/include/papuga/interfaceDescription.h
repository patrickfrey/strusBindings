/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
/// \brief Bindings language interface description
/// \file interfaceDescription.h
#ifndef _PAPUGA_INTERFACE_DESCRIPTION_H_INCLUDED
#define _PAPUGA_INTERFACE_DESCRIPTION_H_INCLUDED
#include <stdbool.h>

/// \brief Structure describing a method of a host object class
typedef struct papuga_MethodDescription
{
	const char* name;				///< name of the method
	const char* funcname;				///< function name of the method
	bool self;					///< method that requires an instance of its class (self pointer)
	int argc;					///< maximum number of arguments
} papuga_MethodDescription;

/// \brief Structure describing a host object class
typedef struct papuga_ClassDescription
{
	unsigned int id;				///< id of the class (unique index counted from 0)
	const char* name;				///< name of class
	const char* funcname_constructor;		///< function name of the constructor
	const char* funcname_destructor;		///< function name of the destructor
	const papuga_MethodDescription* methodtable;	///< (NULL,0) terminated list of methods
} papuga_ClassDescription;

/// \brief Structure describing the interface
typedef struct papuga_InterfaceDescription
{
	const char* name;				///< name of the project wrapped by the bindings
	const char** includefiles;			///< null terminated list of files to include
	const papuga_ClassDescription* classes;		///< (0,NULL,NULL) terminated list of classes 
} papuga_InterfaceDescription;

#endif

