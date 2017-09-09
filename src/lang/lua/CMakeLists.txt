cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

include_directories(
	"${Intl_INCLUDE_DIRS}"
	"${CMAKE_BINARY_DIR}/include"
	"${PROJECT_SOURCE_DIR}/3rdParty/papuga/3rdParty/lua"
	"${PROJECT_SOURCE_DIR}/3rdParty/papuga/include"
	"${PROJECT_SOURCE_DIR}/include"
)

link_directories(
	"${CMAKE_BINARY_DIR}/3rdParty/papuga/3rdParty/lua_cmake"
	"${CMAKE_BINARY_DIR}/3rdParty/papuga/src"
	"${CMAKE_BINARY_DIR}/src/bindings"
	"${CMAKE_BINARY_DIR}/src/lang/lua"
	"${strusbase_LIBRARY_DIRS}"
	"${strusmodule_LIBRARY_DIRS}"
	"${strusrpc_LIBRARY_DIRS}"
	"${strus_LIBRARY_DIRS}"
	"${strusanalyzer_LIBRARY_DIRS}"
	"${strustrace_LIBRARY_DIRS}"
)

add_custom_command(
    OUTPUT     ${CMAKE_BINARY_DIR}/include/strus/lib/lua.h
    DEPENDS   ${CMAKE_BINARY_DIR}/include/strus/bindingObjects.h strusBindingsModuleGen strus_bindings_description
    COMMAND  ${CMAKE_BINARY_DIR}/src/gen/strusBindingsModuleGen lua header ${CMAKE_BINARY_DIR}/include/strus/lib/lua.h
    COMMENT  "Generating Lua module header include/strus/lib/lua.h"
    VERBATIM
 )

add_custom_command(
    OUTPUT     libstrus_bindings_lua.c
    DEPENDS    ${CMAKE_BINARY_DIR}/include/strus/bindingObjects.h strusBindingsModuleGen strus_bindings_description
    COMMAND  ${CMAKE_BINARY_DIR}/src/gen/strusBindingsModuleGen lua module libstrus_bindings_lua.c include=strus/lib/lua.h
    COMMENT  "Generating Lua module source libstrus_bindings_lua.c"
    VERBATIM
 )
 
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/src/lang/lua/strus.lua
    DEPENDS   ${CMAKE_BINARY_DIR}/include/strus/bindingObjects.h strus_bindings_description strusBindingsModuleGen
    COMMAND  ${CMAKE_BINARY_DIR}/src/gen/strusBindingsModuleGen lua doc ${CMAKE_BINARY_DIR}/src/lang/lua/strus.lua
    COMMENT  "Generating bindings interface doc description file ${CMAKE_BINARY_DIR}/src/lang/lua/strus.lua"
    VERBATIM
 )

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/doc/luaBindingsDoc.htm
    DEPENDS   papugaDoc  "${PROJECT_SOURCE_DIR}/src/lang/lua/strusLuaDoc.tpl"  ${CMAKE_BINARY_DIR}/src/lang/lua/strus.lua
    COMMAND  papugaDoc -o ${CMAKE_BINARY_DIR}/doc/luaBindingsDoc.htm "${PROJECT_SOURCE_DIR}/src/lang/lua/strusLuaDoc.tpl" ${CMAKE_BINARY_DIR}/src/lang/lua/strus.lua
    COMMENT  "Generating bindings interface doc html file ${CMAKE_BINARY_DIR}/src/lang/lua/strus.lua"
    VERBATIM
 )

 add_custom_target(
    lua_mod ALL
    DEPENDS ${CMAKE_BINARY_DIR}/include/strus/lib/lua.h libstrus_bindings_lua.c ${CMAKE_BINARY_DIR}/src/lang/lua/strus.lua papuga_lua_gen ${CMAKE_BINARY_DIR}/doc/luaBindingsDoc.htm
)

# ------------------------------
# LIBRARIES
# ------------------------------
add_library( strus_bindings_lua SHARED libstrus_bindings_lua.c )
add_dependencies( strus_bindings_lua lua_mod )

target_link_libraries( strus_bindings_lua papuga_devel papuga_lua_dev strus_bindings strus_lua )

# ------------------------------
# PROGRAMS
# ------------------------------
add_executable( strusLua strusLua.c )
target_link_libraries( strusLua  strus_bindings_lua papuga_devel strus_lua m dl ${Intl_LIBRARIES} )

# FreeBSD needs kernel data access library for libuv (-libkvm)
find_library( LIBKVM_LIBRARIES kvm )
if(LIBKVM_LIBRARIES)
        target_link_libraries( strusLua ${LIBKVM_LIBRARIES} )
endif()

# ------------------------------
# INSTALLATION
# ------------------------------
install( TARGETS strus_bindings_lua
           LIBRARY DESTINATION ${LIB_INSTALL_DIR}/strus
           RUNTIME DESTINATION bin )

install( TARGETS strusLua
           RUNTIME DESTINATION bin )


