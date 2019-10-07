/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Part of the implementation of the context for executing XML/JSON requests on the strus bindings, functions for error handling
/// \file "webRequestContext_error.cpp"
#include "webRequestContext.hpp"
#include "strus/webRequestLoggerInterface.hpp"
#include "strus/base/utf8.hpp"
#include "schemas_base.hpp"
#include "papuga/allocator.h"
#include "papuga/errors.h"
#include "papuga/request.h"
#include "papuga/typedefs.h"
#include "private/internationalization.hpp"
#include <string>

using namespace strus;

void WebRequestContext::reportRequestError( const papuga_RequestError& errstruct, const WebRequestContent& content)
{
	if (errstruct.classname)
	{
		if (errstruct.methodname)
		{
			if (errstruct.argcnt > 0)
			{
				papuga_ErrorBuffer_reportError( &m_errbuf, "%s in method %s::%s argument %d", papuga_ErrorCode_tostring( errstruct.errcode), errstruct.classname, errstruct.methodname, errstruct.argcnt);
			}
			else
			{
				papuga_ErrorBuffer_reportError( &m_errbuf, "%s in method %s::%s", papuga_ErrorCode_tostring( errstruct.errcode), errstruct.classname, errstruct.methodname);
			}
		}
		else
		{
			if (errstruct.argcnt > 0)
			{
				papuga_ErrorBuffer_reportError( &m_errbuf, "%s in constructor of %s", papuga_ErrorCode_tostring( errstruct.errcode), errstruct.classname);
			}
			else
			{
				papuga_ErrorBuffer_reportError( &m_errbuf, "%s in constructor of %s", papuga_ErrorCode_tostring( errstruct.errcode), errstruct.classname, errstruct.argcnt);
			}
		}
	}
	else
	{
		papuga_ErrorBuffer_reportError( &m_errbuf, "%s", papuga_ErrorCode_tostring( errstruct.errcode));
	}
	if (errstruct.variable)
	{
		papuga_ErrorBuffer_appendMessage( &m_errbuf, ", accessing variable '%s'", errstruct.variable);
	}
	if (errstruct.itemid > 0)
	{
		const char* itemname = webrequest::AutomatonNameSpace::itemName( (webrequest::AutomatonNameSpace::Item)errstruct.itemid);
		if (errstruct.structpath[0])
		{
			papuga_ErrorBuffer_appendMessage( &m_errbuf, ", resolving '%s' in %s", itemname, errstruct.structpath);
		}
		else
		{
			papuga_ErrorBuffer_appendMessage( &m_errbuf, " resolving '%s'", itemname);
		}
	}
	if (errstruct.errormsg[0])
	{
		papuga_ErrorBuffer_appendMessage( &m_errbuf, ", message: %s", errstruct.errormsg);
	}
	if (errstruct.scopestart > 0)
	{
		papuga_Allocator allocator;
		char allocator_mem[ 4096];
		papuga_init_Allocator( &allocator, allocator_mem, sizeof(allocator_mem));

		papuga_ErrorCode errcode;
		int locinfolen;
		const char* locinfo = papuga_request_content_tostring( &allocator, m_doctype, m_encoding, content.str(), content.len(), errstruct.scopestart, 3/*max depth*/, &locinfolen, &errcode);
		if (locinfo)
		{
			std::string locinfobuf;
			if (locinfolen > MaxLogContentSize)
			{
				std::size_t endidx = MaxLogContentSize;
				for (;endidx > 0 && strus::utf8midchr( locinfo[ endidx-1]); --endidx){}
				locinfobuf.append( locinfo, endidx);
				locinfobuf.append( " ...");
				locinfo = locinfobuf.c_str();
			}
			papuga_ErrorBuffer_appendMessage( &m_errbuf, " error scope: %s", locinfo);
		}
	}
	if (0!=(m_logger->logMask() & WebRequestLoggerInterface::LogError))
	{
		m_logger->logError( m_errbuf.ptr);
	}
}


