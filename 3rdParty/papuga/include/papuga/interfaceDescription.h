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

typedef struct papuga_MethodDescription
{
	const char* name;				///< name of the method
	const char* funcname;				///< function name of the method
	bool self;					///< method that requires an instance of its class (self pointer)
	int argc;					///< maximum number of arguments
} papuga_MethodDescription;

typedef struct papuga_ClassDescription
{
	unsigned int id;				///< id of the class (unique index counted from 0)
	const char* name;				///< name of class
	const char* constructorname;			///< name of the constructor
	const char* destructorname;			///< name of the destructor
	const papuga_MethodDescription* methodtable;	///< (NULL,0) terminated list of methods
} papuga_ClassDescription;

typedef struct papuga_ErrorDescription
{
	int errorcode;					///< error code
	const char* text;				///< error message string assigned to errorcode
} papuga_ErrorDescription;

typedef struct papuga_InterfaceDescription
{
	const char* name;				///< name of the project wrapped by the bindings
	const char* includefile;			///< name of the bindings interface file to include
	const papuga_ClassDescription* classes;		///< (0,NULL,NULL) terminated list of classes 
	const papuga_ErrorDescription* errors;		///< text for error codes defined in typedefs.h (papuga_ErrorCode)
} papuga_InterfaceDescription;

#endif

