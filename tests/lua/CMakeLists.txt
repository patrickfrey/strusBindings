cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

set( LUA_TESTDIR "${PROJECT_SOURCE_DIR}/tests/lua" )
set( LUA_DATADIR "${PROJECT_SOURCE_DIR}/tests/data" )
set( LUA_EXECDIR "${CMAKE_CURRENT_BINARY_DIR}/exec" )
add_custom_target( LuaTestExecDir ALL COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/exec" )

macro(add_lua_test name args )
    add_test( Lua${name} ${CMAKE_BINARY_DIR}/src/lang/lua/strusLua  "${PROJECT_SOURCE_DIR}/tests/lua/test${name}.lua" ${args} )
    set_property( TEST Lua${name} PROPERTY ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/3rdParty/papuga/lua:${CMAKE_BINARY_DIR}/src/bindings:$ENV{LD_LIBRARY_PATH}")
endmacro(add_lua_test)

add_lua_test( HelloWorld "" )
add_lua_test( CreateCollection_t3s  "${LUA_DATADIR}/t3s"  "${LUA_EXECDIR}" )
add_lua_test( Query_t3s "${LUA_DATADIR}/t3s"  "${LUA_EXECDIR}" )
add_lua_test( CreateCollection_mdprim "${LUA_DATADIR}/mdprim" "${LUA_EXECDIR}" )
add_lua_test( Query_mdprim "${LUA_DATADIR}/mdprim"  "${LUA_EXECDIR}" )
add_lua_test( Vectors "${LUA_EXECDIR}" )

# add_test( LuaHelloWorld ${CMAKE_BINARY_DIR}/src/lang/lua/strusLua  "${PROJECT_SOURCE_DIR}/tests/lua/testHelloWorld.lua" )
# add_test( LuaCreateCollection_t3s ${CMAKE_BINARY_DIR}/src/lang/lua/strusLua  "${PROJECT_SOURCE_DIR}/tests/lua/testCreateCollection_t3s.lua"  "${PROJECT_SOURCE_DIR}/tests/data/t3s" "${CMAKE_BINARY_DIR}/tests/lua/exec" )
# add_test( LuaQuery_t3s ${CMAKE_BINARY_DIR}/src/lang/lua/strusLua  "${PROJECT_SOURCE_DIR}/tests/lua/testQuery_t3s.lua"  "${PROJECT_SOURCE_DIR}/tests/data/t3s" "${CMAKE_BINARY_DIR}/tests/lua/exec" )
# add_test( LuaCreateCollection_mdprim ${CMAKE_BINARY_DIR}/src/lang/lua/strusLua  "${PROJECT_SOURCE_DIR}/tests/lua/testCreateCollection_mdprim.lua"  "${PROJECT_SOURCE_DIR}/tests/data/mdprim" "${CMAKE_BINARY_DIR}/tests/lua/exec" )
# add_test( LuaQuery_mdprim ${CMAKE_BINARY_DIR}/src/lang/lua/strusLua  "${PROJECT_SOURCE_DIR}/tests/lua/testQuery_mdprim.lua"  "${PROJECT_SOURCE_DIR}/tests/data/mdprim" "${CMAKE_BINARY_DIR}/tests/lua/exec" )
# add_test( LuaVectors ${CMAKE_BINARY_DIR}/src/lang/lua/strusLua  "${PROJECT_SOURCE_DIR}/tests/lua/testVectors.lua"  "${CMAKE_BINARY_DIR}/tests/lua/exec" )



