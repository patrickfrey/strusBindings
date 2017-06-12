/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDINGS_PAPUGA_HOST_OBJECT_HPP_INCLUDED
#define _STRUS_BINDINGS_PAPUGA_HOST_OBJECT_HPP_INCLUDED
/// \brief Reference to host object with C++ templates for typesafety
/// \file hostObject.hpp
#include "papuga/hostObject.h"
#include "bindingClassTemplate.hpp"
#include "internationalization.hpp"
#include <stdexcept>

#define PAPUGA_USE_RTTI_TYPECHECK

namespace strus {
namespace bindings {

struct HostObject
{
public:
	/// \brief Destructor
	~HostObject(){}

	/// \brief Create pointer as reference with ownership and reference counting
	template <class OBJECTTYPE>
	static void initOwnership( papuga_HostObject& ref, OBJECTTYPE* obj)
	{
		papuga_init_HostObject( &ref, obj, BindingClassTemplate<OBJECTTYPE>::getDestructor());
	}

	/// \brief Create pointer as constant reference
	template <class OBJECTTYPE>
	static void initConst( papuga_HostObject& ref, const OBJECTTYPE* obj)
	{
		papuga_init_HostObject( &ref, obj, 0);
	}

	/// \brief Get const pointer to object
	template <class OBJECTTYPE>
	static const OBJECTTYPE* getObject( const papuga_HostObject& ref)
	{
		const OBJECTTYPE* obj = static_cast<const OBJECTTYPE*>( (const void*)ref.data);
		if (!obj) return 0;
#ifdef PAPUGA_USE_RTTI_TYPECHECK
		if (dynamic_cast<const OBJECTTYPE*>( obj) == 0)
		{
			throw std::runtime_error( _TXT("internal: unexpected object type passed in bindings"));
		}
#endif
		return obj;
	}

	/// \brief Get pointer to object
	template <class OBJECTTYPE>
	static OBJECTTYPE* getObject( const papuga_HostObject& ref)
	{
		OBJECTTYPE* obj = static_cast<OBJECTTYPE*>( (const void*)ref.data);
		if (!obj) return 0;
#ifdef PAPUGA_USE_RTTI_TYPECHECK
		if (dynamic_cast<OBJECTTYPE*>( obj) == 0)
		{
			throw std::runtime_error( _TXT("internal: unexpected object type passed in bindings"));
		}
#endif
		return obj;
	}
};

}} //namespace
#endif

