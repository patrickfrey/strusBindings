/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
/// \brief Bindings language interface
/// \file languageInterface.hpp
#ifndef _PAPUGA_LANGUAGE_INTERFACE_HPP_INCLUDED
#define _PAPUGA_LANGUAGE_INTERFACE_HPP_INCLUDED
#include <string>

namespace papuga {

class LanguageInterface
{
public:
	struct InterfaceDescription
	{
		struct Method
		{
			const char* name;		///< name of the method
			const char* funcname;		///< function name of the method
			bool self;			///< method that requires an instance of its class (self pointer)
			int argc;			///< maximum number of arguments
		};
		
		struct Class
		{
			unsigned int id;		///< id of the class (unique index counted from 0)
			const char* name;		///< name of class
			const char* constructorname;	///< name of the constructor
			const char* destructorname;	///< name of the destructor
			const Method* methodtable;	///< (NULL,0) terminated list of methods
		};

		struct Details
		{
			unsigned int max_argc;
		};

		struct ErrorText
		{
			int errorcode;
			const char* text;
		};

		const char* name;			///< name of the project wrapped by the bindings
		const Class* classes;			///< (0,NULL,NULL) terminated list of classes 
		const ErrorText* errors;		///< text for error codes defined in typedefs.h (papuga_ErrorCode)
		Details details;			///< some addiitional info that is useful for the interface generator
	};

	virtual ~LanguageInterface(){}

	virtual std::string generate(
			const char* name,
			const InterfaceDescription* descr)=0;
};

}//namespace
#endif

