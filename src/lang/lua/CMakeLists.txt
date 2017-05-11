cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

add_custom_command(
    OUTPUT     strus_lua.h
    DEPENDS   ${PROJECT_SOURCE_DIR}/include/strus/bindingObjects.h
    COMMAND  ${CMAKE_BINARY_DIR}/src/strusBindingsModuleGen lua header strus_lua.h
    COMMENT  "Generating Lua module header"
    VERBATIM
 )

add_custom_command(
    OUTPUT     libstrus_bindings_lua.c
    DEPENDS    ${PROJECT_SOURCE_DIR}/include/strus/bindingObjects.h
    COMMAND  ${CMAKE_BINARY_DIR}/src/strusBindingsModuleGen lua module libstrus_bindings_lua.c
    COMMENT  "Generating Lua module source"
    VERBATIM
 )
 
 add_custom_target(
    lua_mod ALL
    DEPENDS strus_lua.h libstrus_bindings_lua.c
)

add_library( strus_bindings_lua SHARED libstrus_bindings_lua.c )
add_dependencies( strus_bindings_lua lua_mod )

target_link_libraries( strus_bindings_lua strus_bindingsimpl strus_bindings_lua_impl )

install( TARGETS strus_bindings_lua
           LIBRARY DESTINATION ${LIB_INSTALL_DIR}/strus
           RUNTIME DESTINATION bin )

