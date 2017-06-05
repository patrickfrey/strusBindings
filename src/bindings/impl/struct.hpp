/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Interface to arbitrary structure as return value
#ifndef _STRUS_BINDING_IMPL_STRUCT_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_STRUCT_HPP_INCLUDED
#include "papuga/serialization.h"
#include "papuga/allocator.h"
#include <string>
#include <stdexcept>

namespace strus {
namespace bindings {

/// \brief Object representing an arbitrary structure as return value
struct Struct
{
public:
	/// \brief Constructor
	Struct()
		:released(false)
	{
		papuga_init_Serialization( &serialization);
		papuga_init_Allocator( &allocator);
	}

	/// \brief "Move" constructor
	Struct( const Struct& o)
		:released(o.released)
	{
		// PF:HACK: We would like to have a move constructor only,
		// but move semantics are not available in C++98, we should switch to C++11
		if (released)
		{
			memcpy( &serialization, &o.serialization, sizeof(serialization));
			memcpy( &allocator, &o.allocator, sizeof(allocator));
		}
		else
		{
			throw std::logic_error( "deep copy of Struct not allowed");
		}
	}

	/// \brief Destructor
	~Struct()
	{
		if (!released)
		{
			papuga_destroy_Serialization( &serialization);
			papuga_destroy_Allocator( &allocator);
		}
	}

	void release()
	{
		released = true;
	}

	papuga_Serialization serialization;
	papuga_Allocator allocator;

private:
	bool released;
};

}}//namespace
#endif

