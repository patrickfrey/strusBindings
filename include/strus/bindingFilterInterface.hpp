/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_FILTER_INTERFACE_HPP_INCLUDED
#define _STRUS_BINDING_FILTER_INTERFACE_HPP_INCLUDED
/// \brief Iterator on the abstraction of an object in the scripting language
/// \file bindingFilterInterface.hpp
#include "strus/base/stdint.h"
#include "strus/binding/valueVariant.hpp"
#include <cstring>

namespace strus {

/// \brief Iterator on the abstraction of an object in the scripting language
class BindingFilterInterface
{
public:
	/// \brief Maximum Memory a BindingFilterInterface implementation should use
	/// \note Filters should only use pointers to the native data structures of the binding language
	/// \note The restriction allows new on local memory passed to the language binding with createCopy
	enum {MaxBindingFilterMemSize=128};
	typedef struct {char _[MaxBindingFilterMemSize];} BindingFilterMem;

	///\brief Destructor
	virtual ~BindingFilterInterface(){}

	/// \brief Structure tag identifier
	///        Struct := Open E1 E2 ... Close
	///        Atom   := Value
	enum Tag
	{
		Open,		///< Open a scope
		Index,		///< Open an array element
		Value,		///< Atomic value
		Close		///< Closes a scope
	};

	/// \brief Get current element and skip to next
	/// \param[out] current element value
	/// \return current element tag or Close if no elements left
	virtual Tag getNext( binding::ValueVariant& val)=0;

	/// \brief Skip to the close of the current scope
	/// \note This means skipping to the next close that increments the number of 'Close' found by one more than the number of 'Open' found
	virtual void skip()=0;

	/// \brief Create a copy of the current filter state
	/// \return a copy of the current filter state
	virtual BindingFilterInterface* createCopy() const=0;
};

}//namespace
#endif

