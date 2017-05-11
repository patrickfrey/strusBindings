/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
/// \brief Library interface for lua bindings built by papuga
/// \file papuga/lib/lua_dev.h
#ifndef _PAPUGA_LUA_DEV_LIB_H_INCLUDED
#define _PAPUGA_LUA_DEV_LIB_H_INCLUDED
#include "papuga/typedefs.h"
#include <setjmp.h>

#define papuga_LUA_MAX_NOF_ARGUMENTS 64

typedef struct papuga_lua_ClassDescriptor
{
	void (*destructor)( void* self);
	const char* metatablename;
} papuga_lua_ClassDescriptor;

typedef struct papuga_lua_UserData
{
	int classid;
	void* object;
} papuga_lua_UserData;

typedef struct papuga_lua_CallArgs
{
	int erridx;
	int errcode;
	size_t argc;
	size_t serc;
	void* self;
	papuga_ValueVariant argv[ papuga_LUA_MAX_NOF_ARGUMENTS];
	papuga_Serialization serializations[ papuga_LUA_MAX_NOF_ARGUMENTS];
} papuga_lua_CallArgs;

/// \brief Defines some globals needed
bool papuga_init_lua_state( lua_State *ls);

/// \brief Invokes a lua error exception on a papuga error
void papuga_lua_error( lua_State* ls, const char* function, papuga_ErrorCode err);
/// \brief Invokes a lua error exception on a host function execution error
void papuga_lua_error_str( lua_State* ls, const char* function, const char* errormsg);

/// \brief Function that fills a structure with the arguments passed in the lua context for papuga
bool papuga_lua_init_CallArgs( lua_State *ls, papuga_lua_CallArgs* arg, const char* classname);

/// \brief Frees the arguments of a papuga call (to call after the call)
void papuga_lua_destroy_CallArgs( papuga_lua_CallArgs* arg);

/// \brief Function that transfers the call result of a function into the lua context, freeing the call result structure
/// \return the number of values to return
int papuga_lua_move_CallResult( lua_State *ls, papuga_CallResult* callres, papuga_ErrorCode* errcode);

#endif

