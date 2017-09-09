/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Interface to an iterator on an object
#ifndef _STRUS_BINDING_IMPL_ITERATOR_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_ITERATOR_HPP_INCLUDED
#include "papuga/iterator.h"

namespace strus {
namespace bindings {

/// \brief Object representing an iterator closure
struct Iterator
{
public:
	/// \brief Default constructor
	Iterator()
	{
		papuga_init_Iterator( &iterator, 0, 0, 0);
	}
	/// \brief Constructor
	Iterator( void* data, papuga_Deleter deleter, papuga_GetNext getNext)
	{
		papuga_init_Iterator( &iterator, data, deleter, getNext);
	}

	/// \brief "Move" constructor
	Iterator( const Iterator& o)
		:released(o.released)
	{
		// PF:HACK: We would like to have a move constructor only,
		// but move semantics are not available in C++98, we should switch to C++11
		if (released)
		{
			memcpy( &iterator, &o.iterator, sizeof(iterator));
		}
		else
		{
			throw std::logic_error( "deep copy of Iterator not allowed");
		}
	}

	/// \brief Destructor
	~Iterator()
	{
		if (!released)
		{
			papuga_destroy_Iterator( &iterator);
		}
	}

	void release()
	{
		released = true;
	}

	papuga_Iterator iterator;

private:
	bool released;
};

}}//namespace
#endif

