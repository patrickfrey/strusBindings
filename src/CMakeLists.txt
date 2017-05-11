cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

# --------------------------------------
# SOURCES AND INCLUDES
# --------------------------------------
# Subdirectories:
add_subdirectory( parser )

file( MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/include/strus/lib )

add_custom_command(
    OUTPUT   
                ${CMAKE_BINARY_DIR}/include/strus/bindingObjects.h
		${CMAKE_BINARY_DIR}/include/strus/lib/bindings_description.hpp
	        ${CMAKE_BINARY_DIR}/src/bindingObjects.cpp
	        ${CMAKE_BINARY_DIR}/src/libstrus_bindings_description.cpp
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/impl/context.hpp
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/impl/storage.hpp
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/impl/statistics.hpp
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/impl/vector.hpp
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/impl/analyzer.hpp
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/impl/query.hpp
    COMMAND  ${CMAKE_BINARY_DIR}/src/parser/strusBindingsInterfaceGen ${CMAKE_BINARY_DIR}  ${PROJECT_SOURCE_DIR}/src/impl/context.hpp ${PROJECT_SOURCE_DIR}/src/impl/storage.hpp ${PROJECT_SOURCE_DIR}/src/impl/statistics.hpp ${PROJECT_SOURCE_DIR}/src/impl/vector.hpp ${PROJECT_SOURCE_DIR}/src/impl/analyzer.hpp ${PROJECT_SOURCE_DIR}/src/impl/query.hpp
    COMMENT  "Generating strus bindings C header and implementation (output root dir: ${CMAKE_BINARY_DIR})"
    VERBATIM
 )
 
 add_custom_target(
    strus_bindings_c_interface ALL
    DEPENDS ${CMAKE_BINARY_DIR}/src/bindingObjects.cpp ${CMAKE_BINARY_DIR}/include/strus/bindingObjects.h
)

set_source_files_properties( ${CMAKE_BINARY_DIR}/src/bindingObjects.cpp PROPERTIES GENERATED TRUE ) 
set_source_files_properties( ${CMAKE_BINARY_DIR}/include/strus/bindingObjects.h PROPERTIES GENERATED TRUE ) 
set_source_files_properties( ${CMAKE_BINARY_DIR}/include/strus/lib/bindings_description.hpp PROPERTIES GENERATED TRUE ) 
set_source_files_properties( ${CMAKE_BINARY_DIR}/src/libstrus_bindings_description.cpp PROPERTIES GENERATED TRUE ) 

set( bindingsimpl_source_files
	expressionBuilder.cpp
	patternMatcherLoader.cpp
	structNameMap.cpp
	structDefs.cpp
	metadataop.cpp
	serializer.cpp
	deserializer.cpp
	papugaErrorException.cpp
	valueVariantWrap.cpp
	utils.cpp
	traceUtils.cpp
	internationalization.cpp
	impl/analyzer.cpp
	impl/query.cpp
	impl/storage.cpp
        impl/statistics.cpp
	impl/vector.cpp
	impl/context.cpp
)

set( bindings_source_files
	bindingObjects.cpp
)

include_directories(
	"${Boost_INCLUDE_DIRS}"
	"${Intl_INCLUDE_DIRS}"
	"${PROJECT_SOURCE_DIR}/3rdParty/lua"
	"${PROJECT_SOURCE_DIR}/3rdParty/textwolf/include"
	"${PROJECT_SOURCE_DIR}/3rdParty/papuga/include"
	"${PROJECT_SOURCE_DIR}/include"
	"${PROJECT_SOURCE_DIR}/src"
	"${strusbase_INCLUDE_DIRS}"
	"${strus_INCLUDE_DIRS}"
	"${strusanalyzer_INCLUDE_DIRS}"
	"${strusmodule_INCLUDE_DIRS}"
	"${strustrace_INCLUDE_DIRS}"
	"${strusrpc_INCLUDE_DIRS}"
)

link_directories(
	"${PROJECT_SOURCE_DIR}/3rdParty/lua_cmake"
	"${Boost_LIBRARY_DIRS}"
	"${strusbase_LIBRARY_DIRS}"
	"${strusmodule_LIBRARY_DIRS}"
	"${strusrpc_LIBRARY_DIRS}"
)

# ------------------------------
# LIBRARIES
# ------------------------------
add_library( strus_bindingsimpl STATIC ${bindingsimpl_source_files} )
target_link_libraries( strus_bindingsimpl strus_storage_objbuild strus_analyzer_objbuild strus_pattern_serialize strus_module strus_rpc_client strus_rpc_client_socket strus_base strus_error )

add_library( strus_bindings SHARED ${bindings_source_files} )
add_dependencies( strus_bindings  strus_bindings_c_interface )
target_link_libraries( strus_bindings strus_bindingsimpl strus_base strus_error )

add_library( strus_bindings_description SHARED libstrus_bindings_description.cpp )
target_link_libraries( strus_bindings_description )

set_target_properties(
    strus_bindings
    PROPERTIES
    DEBUG_POSTFIX "_d"
    SOVERSION "${STRUS_MAJOR_VERSION}.${STRUS_MINOR_VERSION}"
    VERSION ${STRUS_VERSION}
)

# ------------------------------
# PROGRAMS
# ------------------------------
add_executable( strusLua strusLua.cpp )
target_link_libraries( strusLua  strus_lua ${Intl_LIBRARIES} )

add_executable( strusBindingsModuleGen strusBindingsModuleGen.cpp )
target_link_libraries( strusBindingsModuleGen  "${Boost_LIBRARIES}" strus_base papuga_lua_gen strus_bindings_description ${Intl_LIBRARIES} )

# FreeBSD needs kernel data access library for libuv (-libkvm)
find_library( LIBKVM_LIBRARIES kvm )
if(LIBKVM_LIBRARIES)
        target_link_libraries( strusLua ${LIBKVM_LIBRARIES} )
endif()

add_subdirectory( lang )

# ------------------------------
# INSTALLATION
# ------------------------------
install( TARGETS strus_bindings
           LIBRARY DESTINATION ${LIB_INSTALL_DIR}/strus
           RUNTIME DESTINATION bin )

install( TARGETS strusLua
           RUNTIME DESTINATION bin )

