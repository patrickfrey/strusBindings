static void lua_push_CallResultRef( lua_State *ls, papuga_CallResult* callres)
{
	lua_pushlightuserdata( ls, callres);
	luaL_getmetatable( ls, "_papuga_CallResult");
	lua_setmetatable( ls, -2);
}

static int papuga_lua_panic( lua_State *ls)
{
	lua_getglobal( ls, "_papuga_atpanic");
	lua_CFunction atpanic = lua_tocfunction( ls, -1);
	lua_settop( ls, 0);
	atpanic( ls);
}

DLL_PUBLIC bool papuga_init_lua_state( lua_State *ls)
{
	// Init destructor for CallResult:
	luaL_newmetatable( ls, "_papuga_CallResult");
	lua_pushcfunction( ls, delete_CallResult);
	lua_setfield( m_state, -2, "__gc");
	lua_pop( ls, 1);

	lua_pushcfunction( lua_atpanic( ls, papuga_lua_panic));
	lua_setglobal( ls, "_papuga_atpanic");
}
