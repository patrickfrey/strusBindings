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

typedef struct papuga_lua_CallArgs
{
	int erridx;
	papuga_ErrorCode errcode;
	size_t argc;
	size_t serc;
	void* self;
	papuga_ValueVariant argv[ papuga_LUA_MAX_NOF_ARGUMENTS];
	papuga_Serialization serializations[ papuga_LUA_MAX_NOF_ARGUMENTS];
} papuga_lua_CallArgs;

typedef struct papuga_lua_ClassDef
{
	const char* name;
	papuga_HostObjectDeleter destructor;
} papuga_lua_ClassDef;

typedef struct papuga_lua_ClassDefMap
{
	size_t size;
	const papuga_lua_ClassDef* ar;
} papuga_lua_ClassDefMap;

typedef struct papuga_lua_UserData papuga_lua_UserData;

/// \brief Declares a class with its meta data table
void papuga_lua_declare_class( lua_State* ls, int classid, const char* classname,
				const luaL_Reg* mt, papuga_HostObjectDeleter destructor);

/// \brief Allocate the userdata for a new instance of a class
papuga_lua_UserData* papuga_lua_new_userdata( lua_State* ls, const char* classname);
/// \brief Initialize the userdata of a new class instance created
void papuga_lua_init_UserData( papuga_lua_UserData* udata,
				int classid, void* objref, papuga_HostObjectDeleter destructor);

/// \brief Invokes a lua error exception on a papuga error
void papuga_lua_error( lua_State* ls, const char* function, papuga_ErrorCode err);
/// \brief Invokes a lua error exception on a host function execution error
void papuga_lua_error_str( lua_State* ls, const char* function, const char* errormsg);

/// \brief Function that fills a structure with the arguments passed in the lua context for papuga
bool papuga_lua_init_CallArgs( lua_State *ls, int argc, papuga_lua_CallArgs* arg,
				const char* classname, const papuga_lua_ClassDefMap* classdefmap);

/// \brief Frees the arguments of a papuga call (to call after the call)
void papuga_lua_destroy_CallArgs( papuga_lua_CallArgs* arg);

/// \brief Function that transfers the call result of a function into the lua context, freeing the call result structure
/// \return the number of values to return
int papuga_lua_move_CallResult( lua_State *ls, papuga_CallResult* callres,
				const papuga_lua_ClassDefMap* classdefmap, papuga_ErrorCode* errcode);

#endif

