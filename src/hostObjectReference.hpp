/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _PAPUGA_HOST_OBJECT_REFERENCE_HPP_INCLUDED
#define _PAPUGA_HOST_OBJECT_REFERENCE_HPP_INCLUDED
/// \brief Reference to host object with C++ templates for typesafety
/// \file hostObjectReference.hpp
#include "papuga/hostObjectReference.h"
#include <stdexcept>

#define PAPUGA_USE_RTTI_TYPECHECK

namespace papuga {

struct HostObjectReference
{
public:
	template <class OBJECTTYPE>
	static void deleter( void* obj)
	{
		delete static_cast<OBJECTTYPE*>( obj);
	}
	static void no_deleter( void* obj)
	{}

	/// \brief Destructor
	~HostObjectReference(){}

	/// \brief Create pointer as reference with ownership and reference counting
	template <class OBJECTTYPE>
	static void initOwnership( papuga_HostObjectReference& ref, OBJECTTYPE* obj)
	{
		rt.papuga_init_HostObjectReference( &ref, obj, deleter<OBJECTTYPE>);
	}

	/// \brief Create pointer as constant reference
	template <class OBJECTTYPE>
	static void initConst( papuga_HostObjectReference& ref, const OBJECTTYPE* obj)
	{
		rt.papuga_init_HostObjectReference( &ref, obj, 0);
	}

	/// \brief Get const pointer to object
	template <class OBJECTTYPE>
	static const OBJECTTYPE* getObject( const papuga_HostObjectReference& ref) const
	{
		const OBJECTTYPE* obj = static_cast<const OBJECTTYPE*>( (const void*)ref.data);
		if (!obj) return 0;
#ifdef PAPUGA_USE_RTTI_TYPECHECK
		if (dynamic_cast<const OBJECTTYPE*>( obj) == 0)
		{
			throw std::runtime_error( "internal: unexpected object type passed in bindings");
		}
#endif
		return obj;
	}

	/// \brief Get pointer to object
	template <class OBJECTTYPE>
	static OBJECTTYPE* getObject( const papuga_HostObjectReference& ref)
	{
		OBJECTTYPE* obj = static_cast<OBJECTTYPE*>( (const void*)ref.data);
		if (!obj) return 0;
#ifdef PAPUGA_USE_RTTI_TYPECHECK
		if (dynamic_cast<OBJECTTYPE*>( obj) == 0)
		{
			throw std::runtime_error( "internal: unexpected object type passed in bindings");
		}
#endif
		return obj;
	}
};

} //namespace
#endif

