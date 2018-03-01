/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Interface to arbitrary structure as return value
#include "struct.hpp"
#include "papuga/allocator.h"
#include "strus/errorBufferInterface.hpp"
#include "strus/base/configParser.hpp"
#include "private/internationalization.hpp"
#include "serializer.hpp"
#include "bindingClassTemplate.hpp"
#include <cstring>
#include <stdexcept>

using namespace strus;
using namespace strus::bindings;

Struct::Struct()
	:m_released(false)
{
	papuga_init_Allocator( &allocator, 0, 0);
	papuga_init_Serialization( &serialization, &allocator);
}

Struct::Struct( const Struct& o)
	:m_released(o.m_released)
{
	// PF:HACK: We would like to have a move constructor only,
	// but move semantics are not available in C++98, we should switch to C++11
	if (m_released)
	{
		std::memcpy( &serialization, &o.serialization, sizeof(serialization));
		std::memcpy( &allocator, &o.allocator, sizeof(allocator));
	}
	else
	{
		throw std::logic_error( "deep copy of Struct not allowed");
	}
}

Struct::~Struct()
{
	if (!m_released)
	{
		papuga_destroy_Allocator( &allocator);
	}
}

void Struct::release()
{
	m_released = true;
}

ConfigStruct::ConfigStruct( const std::string& config, ErrorBufferInterface* errorhnd)
{
	typedef std::pair<std::string,std::string> Item;
	typedef std::vector<Item> ItemList;

	Reference<ItemList> cfg( new ItemList( strus::getConfigStringItems( config, errorhnd)));
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("failed to get the configuration: %s"), errorhnd->fetchError());
	}
	strus::bindings::Serializer::serialize( &serialization, *cfg, false/*deep*/);
	if (!papuga_Allocator_alloc_HostObject( &allocator, 0, cfg.get(), strus::bindings::BindingClassTemplate<ItemList>::getDestructor())) throw std::bad_alloc();
	cfg.release();
}

