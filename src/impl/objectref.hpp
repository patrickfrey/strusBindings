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
#include <boost/shared_ptr.hpp>
#include <stdexcept>

#define PAPUGA_USE_RTTI_TYPECHECK

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
		:m_ptr()
	{}

	/// \brief Constructor
	ObjectRef( void* obj, Deleter deleter_)
		:m_ptr()
	{
		m_ptr.reset( obj, deleter_);
	}

	/// \brief Copy constructor
	ObjectRef( const ObjectRef& o)
		:m_ptr(o.m_ptr){}

	/// \brief Destructor
	~ObjectRef(){}

	/// \brief Create pointer as reference with ownership and reference counting
	template <class OBJECTTYPE>
	static ObjectRef createOwnership( OBJECTTYPE* obj)
	{
		ObjectRef rt;
		rt.resetOwnership( obj);
		return rt;
	}

	/// \brief Create pointer as constant reference
	template <class OBJECTTYPE>
	static ObjectRef createConst( const OBJECTTYPE* obj)
	{
		ObjectRef rt;
		rt.resetConst( const_cast<OBJECTTYPE*>(obj));
		return rt;
	}

	/// \brief Define pointer as reference with ownership and reference counting
	template <class OBJECTTYPE>
	void resetOwnership( OBJECTTYPE* obj)
	{
		try
		{
			m_ptr.reset( obj, deleter<OBJECTTYPE>);
		}
		catch (const std::bad_alloc& err)
		{
			delete obj;
			throw std::bad_alloc();
		}
	}

	/// \brief Define pointer as constant reference
	template <class OBJECTTYPE>
	void resetConst( const OBJECTTYPE* obj)
	{
		m_ptr.reset( const_cast<OBJECTTYPE*>(obj), no_deleter);
	}

	/// \brief Clear this
	void reset()
	{
		m_ptr.reset();
	}

	/// \brief Assignment operator
	ObjectRef& operator = (const ObjectRef& o)
	{
		m_ptr = o.m_ptr;
		return *this;
	}

	/// \brief Object access as function
	const void* get() const				{return m_ptr.get();}
	/// \brief Object access as function
	void* get()					{return m_ptr.get();}

	/// \brief Get const pointer to object
	template <class OBJECTTYPE>
	const OBJECTTYPE* getObject() const
	{
		const OBJECTTYPE* obj = static_cast<const OBJECTTYPE*>( (const void*)m_ptr.get());
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
	OBJECTTYPE* getObject()
	{
		OBJECTTYPE* obj = static_cast<OBJECTTYPE*>( m_ptr.get());
		if (!obj) return 0;
#ifdef PAPUGA_USE_RTTI_TYPECHECK
		if (dynamic_cast<OBJECTTYPE*>( obj) == 0)
		{
			throw std::runtime_error( "internal: unexpected object type passed in bindings");
		}
#endif
		return obj;
	}
private:
	boost::shared_ptr<void> m_ptr;
};

}} //namespace
#endif

