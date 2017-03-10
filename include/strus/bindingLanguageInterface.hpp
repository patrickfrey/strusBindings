/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_LANGUAGE_INTERFACE_HPP_INCLUDED
#define _STRUS_BINDING_LANGUAGE_INTERFACE_HPP_INCLUDED
/// \brief Interface for accessing the scripting language types and object descriptions
/// \file bindingLanguageInterface.hpp
#include "strus/bindingFilterInterface.hpp"
#include <cstring>

namespace strus {

/// \brief Interface for accessing the scripting language types and object descriptions
class BindingLanguageInterface
{
public:
	///\brief Destructor
	virtual ~BindingLanguageInterface(){}

	/// \brief Scripting language object reference
	typedef void* LanguageObject;

	/// \brief Create a new object in the scripting language layer and initialize it
	/// \param[in] typname type name of the object created, describes the structure filled
	/// \param[in,out] filter iterator on the structure an member values of the object created
	virtual LanguageObject createLanguageObject( const char* typname, BindingFilterInterface* filter);

	/// \brief Message types that can be reported
	enum MessageType
	{
		ErrorMessage,		///< message is an error
		WarningMessage,		///< message is a warning
		TraceMessage,		///< message is a tracing message
		DebugMessage		///< message is a debug message
	};

	/// \brief Tell the scripting layer to report a message
	/// \param[in] msgType type id of the message
	/// \param[in] msg message to report
	virtual void putMessage( MessageType msgtype, const char* msg);

	/// \brief Exception types that can be thrown
	enum ExceptionType
	{
		RuntimeError,
		BadAlloc,
		LogicError
	};
	/// \brief Force the script to throw an exception when it gets control again
	/// \param[in] exceptionType type id of the exception thrown
	/// \param[in] msg exception message
	virtual void throwException( ExceptionType exceptionType, const char* msg);
};

}//namespace
#endif

