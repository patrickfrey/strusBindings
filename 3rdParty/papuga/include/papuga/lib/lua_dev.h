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
#include "papuga/allocator.h"
#include <setjmp.h>

#define papuga_LUA_MAX_NOF_ARGUMENTS 64

typedef struct papuga_lua_CallArgs
{
	int erridx;
	papuga_ErrorCode errcode;
	size_t argc;
	size_t serc;
	void* self;
	papuga_Allocator allocator;
	papuga_ValueVariant argv[ papuga_LUA_MAX_NOF_ARGUMENTS];
	papuga_Serialization serializations[ papuga_LUA_MAX_NOF_ARGUMENTS];
	int allocbuf[ 1024];
} papuga_lua_CallArgs;

typedef struct papuga_lua_ClassDef
{
	const char* name;
	papuga_Deleter destructor;
} papuga_lua_ClassDef;

typedef struct papuga_lua_ClassDefMap
{
	size_t size;
	const papuga_lua_ClassDef* ar;
} papuga_lua_ClassDefMap;

typedef struct papuga_lua_UserData papuga_lua_UserData;

/// \brief Initialize papuga globals for lua
/// \param[in] ls lua state to initialize
/// \remark this function has to be called when initializing the lua context
void papuga_lua_init( lua_State* ls);

/// \brief Declares a class with its meta data table
/// \param[in] ls lua state context
/// \param[in] classid identifier of the class
/// \param[in] classname unique name of the class
/// \param[in] mt method table of the class
void papuga_lua_declare_class( lua_State* ls, int classid, const char* classname, const luaL_Reg* mt);

/// \brief Allocate the userdata for a new instance of a class
/// \param[in] ls lua state context
/// \param[in] classname unique name of the class
/// \return pointer to the user data allocated
papuga_lua_UserData* papuga_lua_new_userdata( lua_State* ls, const char* classname);

/// \brief Initialize the userdata of a new class instance created
/// \param[in] ls lua state context
/// \param[in] classid identifier of the class
/// \param[in] objref pointer to user data object (class instance)
/// \param[in] destructor destructor of 'objref'
/// \param[in] classdefmap array (map classid) of classes in the application
void papuga_lua_init_UserData( papuga_lua_UserData* udata,
				int classid, void* objref, papuga_Deleter destructor,
				const papuga_lua_ClassDefMap* classdefmap);

/// \brief Invokes a lua error exception on a papuga error
/// \param[in] ls lua state context
/// \param[in] function the caller context of the error
/// \param[in] err error code
void papuga_lua_error( lua_State* ls, const char* function, papuga_ErrorCode err);

/// \brief Invokes a lua error exception on a host function execution error
/// \param[in] ls lua state context
/// \param[in] function the caller context of the error
/// \param[in] errormsg error message string
void papuga_lua_error_str( lua_State* ls, const char* function, const char* errormsg);

/// \brief Function that fills a structure with the arguments passed in the lua context for papuga
/// \param[in] ls lua state context
/// \param[in] argc number of function arguments
/// \param[out] arg argument structure initialized
/// \param[in] classname name of the class of the called method
bool papuga_lua_init_CallArgs( lua_State *ls, int argc, papuga_lua_CallArgs* arg, const char* classname);

/// \brief Frees the arguments of a papuga call (to call after the call)
/// \param[in] arg argument structure freed
void papuga_lua_destroy_CallArgs( papuga_lua_CallArgs* arg);

/// \brief Procedure that transfers the call result of a function into the lua context, freeing the call result structure
/// \param[in] ls lua state context
/// \param[out] callres result structure initialized
/// \param[in] classdefmap table of application class definitions
/// \return the number of values to return
/// \param[out] errcode error code
int papuga_lua_move_CallResult( lua_State *ls, papuga_CallResult* callres,
				const papuga_lua_ClassDefMap* classdefmap, papuga_ErrorCode* errcode);

#endif

