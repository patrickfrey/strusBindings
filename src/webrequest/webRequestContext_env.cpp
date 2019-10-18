/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Part of the implementation of the context for executing XML/JSON requests on the strus bindings, initialization, managing and deinitialization of handler and context objects in the request
/// \file "webRequestContext_result.cpp"
#include "webRequestContext.hpp"
#include "webRequestHandler.hpp"
#include "webRequestUtils.hpp"
#include "strus/webRequestLoggerInterface.hpp"
#include "strus/base/string_format.hpp"
#include "strus/errorCodes.hpp"
#include "papugaLoggerFunctions.hpp"
#include "schemas_base.hpp"
#include "papuga/allocator.h"
#include "papuga/errors.h"
#include "papuga/requestAutomaton.hpp"
#include "papuga/typedefs.h"
#include "private/internationalization.hpp"
#include <string>

using namespace strus;

static bool appendBuf( char* buf, std::size_t bufsize, std::size_t& bufpos, const char* ptr, std::size_t len, ErrorCode& errcode)
{
	if (bufpos + len + 1 >= bufsize)
	{
		errcode = ErrorCodeOutOfMem;
		return false;
	}
	std::memcpy( buf+bufpos, ptr, len);
	buf[ bufpos+=len] = 0;
	return true;
}

struct EnvVariable
{
	const char* name;
	const char* value;
};

static bool parseFormatElement( char* buf, std::size_t bufsize, const char* fmt, const EnvVariable* var, ErrorCode& errcode)
{
	std::size_t bufpos = 0;
	buf[ 0] = 0;

	char const* fi = fmt;
	char const* fn = std::strchr( fi, '{');
	for (; fn; fn=std::strchr( fi,'{'))
	{
		if (!appendBuf( buf, bufsize, bufpos, fi, fn-fi, errcode)) return false;
		fi = fn+1;
		char const* fe = std::strchr( fi, '}');
		if (!fe) {errcode = ErrorCodeSyntax; return false;}
		EnvVariable const* vi = var;
		for (; vi->name; ++vi)
		{
			if (vi->name[0] == fi[0] && 0==std::memcmp( vi->name, fi, fe-fi) && vi->name[ fe-fi] == 0)
			{
				if (!appendBuf( buf, bufsize, bufpos, vi->value, std::strlen(vi->value), errcode)) return false;
				break;
			}
		}
		if (!vi->name)
		{
			errcode = ErrorCodeNotImplemented;
			return false;
		}
		fi = fe+1;
	}
	if (!appendBuf( buf, bufsize, bufpos, fi, std::strlen(fi), errcode)) return false;
	return true;
}

bool WebRequestContext::initSchemaEnvAssignments()
{
	const papuga_RequestEnvAssignment* envar = papuga_RequestAutomation_get_env_assignments( m_atm);
	if (envar)
	{
		int ei = 0;

		for (; envar[ei].variable; ++ei)
		{
			char buf[ 256];
			papuga_ValueVariant value;
			ErrorCode errcode;

			switch (envar[ei].envid)
			{
				case webrequest::AutomatonNameSpace::EnvFormat:
				{
					const EnvVariable varar[] = {
						{"name", m_contextName},
						{"type", m_contextType},
						{"id", m_handler->serviceName()},
						{NULL,NULL}
					};

					if (!parseFormatElement( buf, sizeof(buf), envar[ei].argument, varar, errcode))
					{
						setAnswer( errcode);
						return false;
					}
					papuga_init_ValueVariant_charp( &value, buf);
					break;
				}
				default:
					setAnswer( ErrorCodeNotImplemented);
					return false;
			}
			if (!papuga_RequestContext_define_variable( m_context.get(), envar[ei].variable, &value))
			{
				papuga_ErrorCode papugaErrcode;
				papugaErrcode = papuga_RequestContext_last_error( m_context.get(), true);
				setAnswer( papugaErrorToErrorCode( papugaErrcode));
				return false;
			}
		}
	}
	return true;
}


