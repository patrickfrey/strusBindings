/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDINGS_CALL_RESULT_HPP_INCLUDED
#define _STRUS_BINDINGS_CALL_RESULT_HPP_INCLUDED
/// \brief Representation of a result of a call to language bindings
/// \file callResult.hpp
#include "strus/bindings/valueVariant.hpp"
#include "strus/bindings/serialization.hpp"
#include "strus/bindings/hostObjectReference.hpp"
#include "strus/base/snprintf.h"
#include <string>
#include <cstring>

namespace strus {
namespace bindings {

class CallResult
{
public:
	CallResult()
		:value(),object(),serialization()
	{
		m_errorbuf[0] = 0;
	}
	CallResult( const CallResult& o)
		:value(o.value),object(o.object),serialization(o.serialization)
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

	void reportError( const char* fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		strus_vsnprintf( m_errorbuf, sizeof(m_errorbuf), fmt, ap);
		va_end(ap);
	}

public:
	ValueVariant value;
	HostObjectReference object;
	Serialization serialization;

public:
	enum {MaxErrorMessageSize=1024};

private:
	char m_errorbuf[ MaxErrorMessageSize];
};

}}//namespace
#endif


