/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Interface to an iterator as return value
#ifndef _STRUS_BINDING_IMPL_ITERATOR_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_ITERATOR_HPP_INCLUDED
#include "papuga/iterator.h"

namespace strus {
namespace bindings {

/// \brief Object representing an iterator as return value
struct Iterator
{
public:
	/// \brief Constructor
	Iterator()
	{
		papuga_init_Iterator( &iterator, 0, 0, 0);
	}

	/// \brief Destructor
	~Iterator(){}

	papuga_Iterator iterator;
};

}}//namespace
#endif

