/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDINGS_HOST_OBJECT_REFERENCE_HPP_INCLUDED
#define _STRUS_BINDINGS_HOST_OBJECT_REFERENCE_HPP_INCLUDED
/// \brief Reference to host object from the scripting language
/// \file strus/bindings/hostObjectReference.hpp
#include <boost/shared_ptr.hpp>
#include <stdexcept>

#define STRUS_USE_RTTI_TYPECHECK

namespace strus {
namespace bindings {

class HostObjectReference
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
	HostObjectReference()
		:m_ptr()
	{}

	/// \brief Constructor
	HostObjectReference( void* obj, Deleter deleter_)
		:m_ptr()
	{
		m_ptr.reset( obj, deleter_);
	}

	/// \brief Copy constructor
	HostObjectReference( const HostObjectReference& o)
		:m_ptr(o.m_ptr){}

	/// \brief Destructor
	~HostObjectReference(){}

	template <class OBJECTTYPE>
	static HostObjectReference createOwnership( OBJECTTYPE* obj)
	{
		HostObjectReference rt;
		rt.resetOwnership( obj);
		return rt;
	}

	template <class OBJECTTYPE>
	static HostObjectReference createConst( const OBJECTTYPE* obj)
	{
		HostObjectReference rt;
		rt.resetConst( const_cast<OBJECTTYPE*>(obj));
		return rt;
	}

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

	template <class OBJECTTYPE>
	void resetConst( const OBJECTTYPE* obj)
	{
		m_ptr.reset( const_cast<OBJECTTYPE*>(obj), no_deleter);
	}

	/// \brief Assignment operator
	HostObjectReference& operator = (const HostObjectReference& o)
	{
		m_ptr = o.m_ptr;
		return *this;
	}

	/// \brief Object access as function
	const void* get() const				{return m_ptr.get();}
	/// \brief Object access as function
	void* get()					{return m_ptr.get();}

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
	boost::shared_ptr<void> m_ptr;
};

}} //namespace
#endif

