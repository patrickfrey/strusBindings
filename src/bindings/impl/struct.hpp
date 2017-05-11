/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Interface to arbitrary structure s return value
#ifndef _STRUS_BINDING_IMPL_STRUCT_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_STRUCT_HPP_INCLUDED
#include "papuga/serialization.h"
#include "papuga/valueVariant.h"
#include <vector>
#include <string>

namespace strus {
namespace bindings {

/// \brief Object used to search for similar vectors in the collection
struct Struct
{
public:
	/// \brief Constructor
	Struct()
	{
		papuga_init_Serialization( &serialization);
	}

	/// \brief Destructor
	~Struct()
	{
		papuga_destroy_Serialization( &serialization);
	}

	papuga_Serialization serialization;
	std::string strings;
};

}}//namespace
#endif

