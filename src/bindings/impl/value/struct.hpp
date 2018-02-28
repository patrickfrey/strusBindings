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
#include "papuga/typedefs.h"
#include <string>

namespace strus {

/// \brief Forward declaration
class ErrorBufferInterface;

namespace bindings {

/// \brief Object representing an arbitrary structure as return value
struct Struct
{
public:
	/// \brief Constructor
	Struct();

	/// \brief "Move" constructor
	Struct( const Struct& o);

	/// \brief Destructor
	~Struct();

	void release();

public:
	papuga_Serialization serialization;
	papuga_Allocator allocator;
private:
	bool released;
};


struct ConfigStruct
	:public Struct
{
	ConfigStruct( const std::string& config, ErrorBufferInterface* errorhnd);
};


}}//namespace
#endif

