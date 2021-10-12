/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* @brief RAII Container for papuga request context
 * @file papugaLuaRequestHandlerRef.hpp
 */
#ifndef _STRUS_WEBREQUEST_PAPUGA_LUA_REQUEST_HANDLER_REF_HPP_INCLUDED
#define _STRUS_WEBREQUEST_PAPUGA_LUA_REQUEST_HANDLER_REF_HPP_INCLUDED
#include "papuga/luaRequestHandler.h"
#include "strus/webRequestDelegateContextInterface.hpp"
#include "strus/reference.hpp"
#include "webRequestUtils.hpp"
#include <iostream>
#include <map>

namespace strus {

class PapugaLuaRequestHandlerDeleter
{
public:
	void operator()( papuga_LuaRequestHandler* p)
	{
		papuga_destroy_LuaRequestHandler(p);
	}
};

class PapugaLuaRequestHandlerScriptDeleter
{
public:
	void operator()( papuga_LuaRequestHandlerScript* p)
	{
		papuga_destroy_LuaRequestHandlerScript(p);
	}
};

typedef strus::Reference<papuga_LuaRequestHandler,PapugaLuaRequestHandlerDeleter> PapugaLuaRequestHandlerRef;
typedef strus::Reference<papuga_LuaRequestHandlerScript,PapugaLuaRequestHandlerScriptDeleter> PagugaLuaRequestHandlerScriptRef;

struct PagugaLuaRequestHandlerScriptKey
{
	char const* str;

	PagugaLuaRequestHandlerScriptKey( char const* str_) :str(str_){}
	PagugaLuaRequestHandlerScriptKey( const PagugaLuaRequestHandlerScriptKey& o) :str(o.str){}
	bool operator < (const PagugaLuaRequestHandlerScriptKey& o) const noexcept {return std::strcmp(str,o.str)<0;}
};

class PagugaLuaRequestHandlerScriptMap
	:public std::map<PagugaLuaRequestHandlerScriptKey,PagugaLuaRequestHandlerScriptRef>
{
public:
	void add( const PagugaLuaRequestHandlerScriptRef& ref)
	{
		const char* name = papuga_LuaRequestHandlerScript_name( ref.get());
		operator[]( name) = ref;
	}
	PagugaLuaRequestHandlerScriptMap& operator = (const PagugaLuaRequestHandlerScriptMap& o)
	{
		std::map<PagugaLuaRequestHandlerScriptKey,PagugaLuaRequestHandlerScriptRef>::operator=(o);
		return *this;
	}
};

}//namespace
#endif
