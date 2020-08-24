/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _PAPUGA_OBJECT_REFERENCE_HPP_INCLUDED
#define _PAPUGA_OBJECT_REFERENCE_HPP_INCLUDED
/// \brief Reference to host object from the scripting language
/// \file ObjectRef.hpp
#include "strus/base/shared_ptr.hpp"
#include <stdexcept>

#define STRUS_USE_RTTI_TYPECHECK
#undef STRUS_LOWLEVEL_DEBUG
#ifdef STRUS_LOWLEVEL_DEBUG
#include <iostream>
#endif

namespace strus {
namespace bindings {

class ObjectRef
{
public:
	typedef void (*Deleter)( void* obj);

	template <class OBJECTTYPE>
	static void deleter( void* obj)
	{
		delete static_cast<OBJECTTYPE*>( obj);
	}
	static void no_deleter( void* obj)
	{}

	/// \brief Default constructor
	ObjectRef()
#ifdef STRUS_LOWLEVEL_DEBUG
		:m_ptr(),m_name("null")
#else
		:m_ptr()
#endif
	{}

	/// \brief Constructor
#ifdef STRUS_LOWLEVEL_DEBUG
	ObjectRef( void* obj, Deleter deleter_, const char* name_)
		:m_ptr(),m_name(name_)
#else
	ObjectRef( void* obj, Deleter deleter_, const char*)
		:m_ptr()
#endif
	{
		m_ptr.reset( obj, deleter_);
	}

	/// \brief Copy constructor
#ifdef STRUS_LOWLEVEL_DEBUG
	ObjectRef( const ObjectRef& o)
		:m_ptr(o.m_ptr),m_name(o.m_name)
	{
		std::cerr << "COPY ObjRef " << m_name << " (" << m_ptr.use_count() << ")" << std::endl;
	}
#else
	ObjectRef( const ObjectRef& o)
		:m_ptr(o.m_ptr){}
#endif

	/// \brief Destructor
	~ObjectRef()
	{
#ifdef STRUS_LOWLEVEL_DEBUG
		std::cerr << "DELETE ObjRef " << m_name << " (" << m_ptr.use_count() << ")" << std::endl;
#endif
	}

	/// \brief Create pointer as reference with ownership and reference counting
	template <class OBJECTTYPE>
	static ObjectRef createOwnership( OBJECTTYPE* obj, const char* name_)
	{
		ObjectRef rt;
		rt.resetOwnership( obj, name_);
		return rt;
	}

	/// \brief Create pointer as constant reference
	template <class OBJECTTYPE>
	static ObjectRef createConst( const OBJECTTYPE* obj, const char* name_)
	{
		ObjectRef rt;
		rt.resetConst( const_cast<OBJECTTYPE*>(obj), name_);
		return rt;
	}

	/// \brief Define pointer as reference with ownership and reference counting
	template <class OBJECTTYPE>
#ifdef STRUS_LOWLEVEL_DEBUG
	void resetOwnership( OBJECTTYPE* obj, const char* name_)
#else
	void resetOwnership( OBJECTTYPE* obj, const char*)
#endif
	{
		try
		{
			if (obj)
			{
				m_ptr.reset( obj, deleter<OBJECTTYPE>);
			}
			else
			{
				m_ptr.reset();
			}
#ifdef STRUS_LOWLEVEL_DEBUG
			m_name = name_;
#endif
		}
		catch (const std::bad_alloc& err)
		{
			delete obj;
			throw std::bad_alloc();
		}
	}

	/// \brief Define pointer as borrowed reference (no deleter) with ownership and reference counting
	template <class OBJECTTYPE>
#ifdef STRUS_LOWLEVEL_DEBUG
	void resetBorrowed( OBJECTTYPE* obj, const char* name_)
#else
	void resetBorrowed( OBJECTTYPE* obj, const char*)
#endif
	{
		m_ptr.reset( obj, no_deleter);
#ifdef STRUS_LOWLEVEL_DEBUG
		m_name = name_;
#endif
	}

	/// \brief Define pointer as constant reference
	template <class OBJECTTYPE>
#ifdef STRUS_LOWLEVEL_DEBUG
	void resetConst( const OBJECTTYPE* obj, const char* name_)
#else
	void resetConst( const OBJECTTYPE* obj, const char*)
#endif
	{
		m_ptr.reset( const_cast<OBJECTTYPE*>(obj), no_deleter);
#ifdef STRUS_LOWLEVEL_DEBUG
		m_name = name_;
#endif
	}

	/// \brief Clear this
	void reset()
	{
#ifdef STRUS_LOWLEVEL_DEBUG
		std::cerr << "DELETE ObjRef " << m_name << " (" << m_ptr.use_count() << ")" << std::endl;
#endif
		m_ptr.reset();
#ifdef STRUS_LOWLEVEL_DEBUG
		m_name = "null";
#endif
	}

	/// \brief Assignment operator
	ObjectRef& operator = (const ObjectRef& o)
	{
		m_ptr = o.m_ptr;
#ifdef STRUS_LOWLEVEL_DEBUG
		m_name = o.m_name;
#endif
		return *this;
	}

	/// \brief Object access as function
	const void* get() const				{return m_ptr.get();}
	/// \brief Object access as function
	void* get()					{return m_ptr.get();}

	/// \brief Object name
#ifdef STRUS_LOWLEVEL_DEBUG
	const char* name() const			{return m_name;}
#else
	const char* name() const			{return 0;}
#endif

	/// \brief Get const pointer to object
	template <class OBJECTTYPE>
	const OBJECTTYPE* getObject() const
	{
		const OBJECTTYPE* obj = static_cast<const OBJECTTYPE*>( (const void*)m_ptr.get());
		if (!obj) return 0;
#ifdef STRUS_USE_RTTI_TYPECHECK
		if (dynamic_cast<const OBJECTTYPE*>( obj) == 0)
		{
			throw std::runtime_error( "internal: unexpected object type passed in bindings");
		}
#endif
		return obj;
	}

	/// \brief Get pointer to object
	template <class OBJECTTYPE>
	OBJECTTYPE* getObject()
	{
		OBJECTTYPE* obj = static_cast<OBJECTTYPE*>( m_ptr.get());
		if (!obj) return 0;
#ifdef STRUS_USE_RTTI_TYPECHECK
		if (dynamic_cast<OBJECTTYPE*>( obj) == 0)
		{
			throw std::runtime_error( "internal: unexpected object type passed in bindings");
		}
#endif
		return obj;
	}
private:
	strus::shared_ptr<void> m_ptr;
#ifdef STRUS_LOWLEVEL_DEBUG
	const char* m_name;
#endif
};

}} //namespace
#endif

