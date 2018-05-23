cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

configure_file( "${PROJECT_SOURCE_DIR}/src/lang/lua/dist.info.in"   "${CMAKE_CURRENT_BINARY_DIR}/dist.info"   @ONLY )
configure_file( "${PROJECT_SOURCE_DIR}/src/lang/lua/strus.lua.in"   "${CMAKE_CURRENT_BINARY_DIR}/strus.lua"   @ONLY )

include_directories(
        ${LUA_INCLUDE_DIRS}
	${Intl_INCLUDE_DIRS}
	${PAPUGA_INCLUDE_DIRS}
	${BINDINGS_INCLUDE_DIRS}
)

link_directories(
	${Boost_LIBRARY_DIRS}
        ${LUA_LIBRARY_DIRS}
        ${PAPUGA_LUA_LIBRARY_DIRS}
	${PAPUGA_LIBRARY_DIRS}
	${BINDINGS_LIBRARY_DIRS}
	${strusbase_LIBRARY_DIRS}
	${strusmodule_LIBRARY_DIRS}
	${strusrpc_LIBRARY_DIRS}
	${strus_LIBRARY_DIRS}
	${strusanalyzer_LIBRARY_DIRS}
	${strustrace_LIBRARY_DIRS}
)

add_custom_command(
    OUTPUT     ${CMAKE_CURRENT_BINARY_DIR}/../../../include/strus/lib/lua.h
    DEPENDS   ${CMAKE_CURRENT_BINARY_DIR}/../../../include/strus/bindingObjects.h strusBindingsModuleGen strus_bindings_description
    COMMAND  strusBindingsModuleGen lua header ${CMAKE_CURRENT_BINARY_DIR}/../../../include/strus/lib/lua.h
    COMMENT  "Generating Lua module header include/strus/lib/lua.h"
    VERBATIM
 )

add_custom_command(
    OUTPUT     libstrus_bindings_lua.c
    DEPENDS    ${CMAKE_CURRENT_BINARY_DIR}/../../../include/strus/bindingObjects.h strusBindingsModuleGen strus_bindings_description
    COMMAND  strusBindingsModuleGen lua module libstrus_bindings_lua.c include=strus/lib/lua.h
    COMMENT  "Generating Lua module source libstrus_bindings_lua.c"
    VERBATIM
 )
 
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/strusdoc.lua
    DEPENDS   ${CMAKE_CURRENT_BINARY_DIR}/../../../include/strus/bindingObjects.h strus_bindings_description strusBindingsModuleGen
    COMMAND  strusBindingsModuleGen lua doc ${CMAKE_CURRENT_BINARY_DIR}/strusdoc.lua
    COMMENT  "Generating bindings interface doc description file ${CMAKE_CURRENT_BINARY_DIR}/strusdoc.lua"
    VERBATIM
 )

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/doc/luaBindingsDoc.htm
    DEPENDS   papugaDoc  "${PROJECT_SOURCE_DIR}/src/lang/lua/strusLuaDoc.tpl"  ${CMAKE_CURRENT_BINARY_DIR}/strusdoc.lua
    COMMAND  papugaDoc -o ${CMAKE_BINARY_DIR}/doc/luaBindingsDoc.htm "${PROJECT_SOURCE_DIR}/src/lang/lua/strusLuaDoc.tpl" ${CMAKE_CURRENT_BINARY_DIR}/strusdoc.lua
    COMMENT  "Generating bindings interface doc html file ${CMAKE_BINARY_DIR}/doc/luaBindingsDoc.htm"
    VERBATIM
 )

configure_file( ${PROJECT_SOURCE_DIR}/src/lang/lua/luaBindingsDoc.css  ${CMAKE_BINARY_DIR}/doc/luaBindingsDoc.css  COPYONLY )
configure_file( ${PROJECT_SOURCE_DIR}/src/lang/lua/luaBindingsDoc.js  ${CMAKE_BINARY_DIR}/doc/luaBindingsDoc.js  COPYONLY )

 add_custom_target(
    lua_mod ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/../../../include/strus/lib/lua.h libstrus_bindings_lua.c ${CMAKE_CURRENT_BINARY_DIR}/strusdoc.lua papuga_lua_gen ${CMAKE_BINARY_DIR}/doc/luaBindingsDoc.htm ${CMAKE_BINARY_DIR}/doc/luaBindingsDoc.css ${CMAKE_BINARY_DIR}/doc/luaBindingsDoc.js )

# ------------------------------
# LIBRARIES
# ------------------------------
add_cppcheck( strus_bindings_lua  libstrus_bindings_lua.c strusLua.c )

add_library( strus_bindings_lua SHARED libstrus_bindings_lua.c )
add_dependencies( strus_bindings_lua lua_mod )
target_link_libraries( strus_bindings_lua papuga_devel papuga_lua_dev strus_bindings papuga_lua ${Intl_LIBRARIES} )

# ------------------------------
# PROGRAMS
# ------------------------------
add_executable( strusLua strusLua.c )
target_link_libraries( strusLua  strus_bindings_lua papuga_devel papuga_lua m ${Intl_LIBRARIES} )

# FreeBSD needs kernel data access library for libuv (-libkvm)
find_library( LIBKVM_LIBRARIES kvm )
if(LIBKVM_LIBRARIES)
        target_link_libraries( strusLua ${LIBKVM_LIBRARIES} )
endif()

find_library( LIBDL_LIBRARY dl )
if(LIBDL_LIBRARY)
        target_link_libraries( strusLua ${LIBDL_LIBRARY} )
endif()

# ------------------------------
# INSTALLATION
# ------------------------------
install( TARGETS strus_bindings_lua
           LIBRARY DESTINATION ${LIB_INSTALL_DIR}/strus
           RUNTIME DESTINATION bin )

install( TARGETS strusLua
           RUNTIME DESTINATION bin )



