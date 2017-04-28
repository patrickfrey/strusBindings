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
			const char* funcname;		///< name of the method
			int argc;			///< maximum number of arguments
		};
		
		struct Class
		{
			unsigned int id;		///< id of the class (unique index counted from 0)
			const char* name;		///< name of class
			const Method* methodtable;	///< (NULL,0) terminated list of methods
		};

		const char* name;			///< name of the project wrapped by the bindings
		const Class* classes;			///< (0,NULL,NULL) terminated list of classes 
	};

	virtual ~LanguageInterface(){}

	virtual std::string generate(
			const char* name,
			const InterfaceDescription* descr)=0;
};

}//namespace
#endif

