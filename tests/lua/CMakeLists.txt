cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

set( LUA_TESTDIR "${PROJECT_SOURCE_DIR}/tests/lua" )
set( LUA_DATADIR "${PROJECT_SOURCE_DIR}/tests/data" )
set( LUA_EXECDIR "${CMAKE_CURRENT_BINARY_DIR}/exec" )
add_custom_target( LuaTestExecDir ALL COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/exec" )

set( TEST_LD_LIBRARY_PATH "${CMAKE_CURRENT_BINARY_DIR}/../../3rdParty/papuga/lua" )
set( TEST_LD_LIBRARY_PATH "${TEST_LD_LIBRARY_PATH}:${CMAKE_CURRENT_BINARY_DIR}/../../src/bindings" )

macro( add_lua_test name )
    add_test( Lua${name} ${CMAKE_CURRENT_BINARY_DIR}/../../src/lang/lua/strusLua  "${PROJECT_SOURCE_DIR}/tests/lua/test${name}.lua"  ${ARGN}  )
    set_property( TEST Lua${name}  APPEND PROPERTY ENVIRONMENT LD_LIBRARY_PATH=${TEST_LD_LIBRARY_PATH} )  
    set_property( TEST Lua${name}  APPEND PROPERTY ENVIRONMENT STRUS_MODULE_PATH=${STRUS_MODULE_PATH} )
endmacro(add_lua_test)

add_lua_test( HelloWorld "" )
add_lua_test( PrintEnv "" )
add_lua_test( LoadStrusExtension "" )
add_lua_test( CreateCollection_t3s  "${LUA_DATADIR}/t3s"  "${LUA_EXECDIR}" )
add_lua_test( Query_t3s "${LUA_DATADIR}/t3s"  "${LUA_EXECDIR}" )
add_lua_test( CreateCollection_mdprim "${LUA_DATADIR}/mdprim" "${LUA_EXECDIR}" )
add_lua_test( Query_mdprim "${LUA_DATADIR}/mdprim"  "${LUA_EXECDIR}" )
IF (WITH_STRUS_VECTOR STREQUAL "YES")
add_lua_test( Vectors "${LUA_EXECDIR}" )
ENDIF (WITH_STRUS_VECTOR STREQUAL "YES")

