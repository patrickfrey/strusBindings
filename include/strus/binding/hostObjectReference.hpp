/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_HOST_OBJECT_REFERENCE_HPP_INCLUDED
#define _STRUS_BINDING_HOST_OBJECT_REFERENCE_HPP_INCLUDED
/// \brief Reference to host object from the scripting language
/// \file strus/binding/hostObjectReference.hpp
#include <boost/shared_ptr.hpp>

namespace strus {
namespace binding {

class HostObjectReference
{
public:
	typedef void (*Deleter)( void* obj);

	/// \brief Default constructor
	explicit HostObjectReference( Deleter deleter_)
		:m_ptr(),m_deleter(deleter_){}

	/// \brief Copy constructor
	HostObjectReference( const HostObjectReference& o)
		:m_ptr(o.m_ptr),m_deleter(o.m_deleter){}

	/// \brief Destructor
	~HostObjectReference(){}

	void reset( void* obj_=0)
	{
		m_ptr.reset( obj_, m_deleter);
	}

	/// \brief Assignment operator
	HostObjectReference& operator = (const HostObjectReference& o)
	{
		m_ptr = o.m_ptr;
		m_deleter = o.m_deleter;
		return *this;
	}

	/// \brief Object access as function
	const void* get() const				{return m_ptr.get();}
	/// \brief Object access as function
	void* get()					{return m_ptr.get();}

private:
	boost::shared_ptr<void> m_ptr;
	void (*m_deleter)( void* obj);
};

}} //namespace
#endif

