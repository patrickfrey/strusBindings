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
#include <iostream>

namespace strus {

class PapugaLuaRequestHandlerDeleter
{
public:
	void operator()( papuga_LuaRequestHandler* p)
	{
		papuga_destroy_LuaRequestHandler(p);
	}
};

class PapugaLuaRequestHandlerRef
	:public strus::Reference<papuga_LuaRequestHandler,PapugaLuaRequestHandlerDeleter>
	,public WebRequestDelegateContextInterface
{
public:
	typedef strus::Reference<papuga_LuaRequestHandler,PapugaLuaRequestHandlerDeleter> Parent;
	PapugaLuaRequestHandlerRef(){}
	explicit PapugaLuaRequestHandlerRef( papuga_LuaRequestHandler* p) :Parent(p){}
	PapugaLuaRequestHandlerRef( const PapugaLuaRequestHandlerRef& o) :Parent(o){}
	~PapugaLuaRequestHandlerRef(){}
	PapugaLuaRequestHandlerRef& operator=( const PapugaLuaRequestHandlerRef& o) {Parent::operator=(o); return *this;}

	void create( papuga_LuaRequestHandler* ptr) {Parent::operator=( Parent( ptr));}

	void putAnswer( const WebRequestAnswer& status)
	{
		void papuga_LuaRequestHandler_init_result( papuga_LuaRequestHandler* handler, int idx, const char* resultstr, size_t resultlen);
		void papuga_LuaRequestHandler_init_error( papuga_LuaRequestHandler* handler, int idx, papuga_ErrorCode errcode, const char* errmsg);
	}
};

}//namespace
#endif

