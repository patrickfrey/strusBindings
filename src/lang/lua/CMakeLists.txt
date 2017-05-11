cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

include_directories(
	"${Intl_INCLUDE_DIRS}"
	"${CMAKE_BINARY_DIR}/include"
	"${PROJECT_SOURCE_DIR}/3rdParty/lua"
	"${PROJECT_SOURCE_DIR}/3rdParty/papuga/include"
	"${PROJECT_SOURCE_DIR}/include"
)

link_directories(
	"${PROJECT_SOURCE_DIR}/3rdParty/lua_cmake"
	"${PROJECT_SOURCE_DIR}/3rdParty/papuga/src"
	"${PROJECT_SOURCE_DIR}/src/bindings"
	"${strusbase_LIBRARY_DIRS}"
	"${strusmodule_LIBRARY_DIRS}"
	"${strusrpc_LIBRARY_DIRS}"
	"${strus_LIBRARY_DIRS}"
	"${strusanalyzer_LIBRARY_DIRS}"
	"${strustrace_LIBRARY_DIRS}"
)

add_custom_command(
    OUTPUT     ${CMAKE_BINARY_DIR}/include/strus/bindings/lua.h
    DEPENDS   ${CMAKE_BINARY_DIR}/include/strus/bindingObjects.h strusBindingsModuleGen
    COMMAND  ${CMAKE_BINARY_DIR}/src/gen/strusBindingsModuleGen lua header ${CMAKE_BINARY_DIR}/include/strus/bindings/lua.h
    COMMENT  "Generating Lua module header"
    VERBATIM
 )

add_custom_command(
    OUTPUT     libstrus_bindings_lua.c
    DEPENDS    ${CMAKE_BINARY_DIR}/include/strus/bindingObjects.h strusBindingsModuleGen
    COMMAND  ${CMAKE_BINARY_DIR}/src/gen/strusBindingsModuleGen lua module libstrus_bindings_lua.c
    COMMENT  "Generating Lua module source"
    VERBATIM
 )
 
 add_custom_target(
    lua_mod ALL
    DEPENDS ${CMAKE_BINARY_DIR}/include/strus/bindings/lua.h libstrus_bindings_lua.c papuga_lua_gen
)

set_source_files_properties( strus_lua.h PROPERTIES GENERATED TRUE ) 
set_source_files_properties( libstrus_bindings_lua.c PROPERTIES GENERATED TRUE ) 

# ------------------------------
# LIBRARIES
# ------------------------------
add_library( strus_bindings_lua SHARED libstrus_bindings_lua.c )
add_dependencies( strus_bindings_lua lua_mod )

target_link_libraries( strus_bindings_lua strus_bindings strus_lua )

# ------------------------------
# PROGRAMS
# ------------------------------
add_executable( strusLua strusLua.cpp )
target_link_libraries( strusLua  strus_lua ${Intl_LIBRARIES} )

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


