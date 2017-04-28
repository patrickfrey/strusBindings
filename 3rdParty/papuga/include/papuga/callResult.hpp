/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _PAPUGA_CALL_RESULT_HPP_INCLUDED
#define _PAPUGA_CALL_RESULT_HPP_INCLUDED
/// \brief Representation of a result of a call to papuga language bindings
/// \file callResult.hpp
#include "papuga/valueVariant.hpp"
#include "papuga/serialization.hpp"
#include "papuga/hostObjectReference.hpp"
#include <string>
#include <cstring>
#include <cstdarg>
#include <cstdio>

namespace papuga {

class CallResult
{
public:
	CallResult()
		:value(),object(),serialization(),stringbuf()
	{
		m_errorbuf[0] = 0;
	}
	CallResult( const CallResult& o)
		:value(o.value),object(o.object),serialization(o.serialization),stringbuf(o.stringbuf)
	{
		if (o.m_errorbuf[0])
		{
			std::strncpy( m_errorbuf, o.m_errorbuf, sizeof(m_errorbuf));
			m_errorbuf[ sizeof(m_errorbuf)-1] = 0;
		}
		else
		{
			m_errorbuf[0] = 0;
		}
	}

	CallResult( int val)
		:value((ValueVariant::IntType)val),object(),serialization(),stringbuf()
		{m_errorbuf[0] = 0;}

	CallResult( unsigned int val)
		:value((ValueVariant::UIntType)val),object(),serialization(),stringbuf()
		{m_errorbuf[0] = 0;}

	CallResult( double val)
		:value(val),object(),serialization(),stringbuf()
		{m_errorbuf[0] = 0;}

	CallResult( const std::string& val)
		:value(),object(),serialization(),stringbuf(val)
		{m_errorbuf[0] = 0; value.init( stringbuf);}

	void reportError( const char* fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		std::vsnprintf( m_errorbuf, sizeof(m_errorbuf), fmt, ap);
		va_end(ap);
	}
	bool hasError() const
	{
		return m_errorbuf[0] != 0;
	}
	const char* lastError() const
	{
		return m_errorbuf;
	}

public:
	ValueVariant value;
	HostObjectReference object;
	Serialization serialization;
	std::string stringbuf;

public:
	enum {MaxErrorMessageSize=1024};

private:
	char m_errorbuf[ MaxErrorMessageSize];
};

}//namespace
#endif


