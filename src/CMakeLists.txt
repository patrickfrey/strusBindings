cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

# --------------------------------------
# SOURCES AND INCLUDES
# --------------------------------------
# Subdirectories:
add_subdirectory( parser )

set( lua_source_files
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lapi.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lauxlib.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lbaselib.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lbitlib.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lcode.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lcorolib.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lctype.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/ldblib.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/ldebug.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/ldo.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/ldump.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lfunc.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lgc.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/linit.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/liolib.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/llex.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lmathlib.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lmem.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/loadlib.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lobject.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lopcodes.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/loslib.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lparser.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lstate.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lstring.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lstrlib.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/ltable.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/ltablib.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/ltests.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/ltm.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lua.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lundump.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lutf8lib.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lvm.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lzio.c
)

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
	"${CMAKE_BINARY_DIR}/3rdParty/papuga/lua/"
	"${Boost_LIBRARY_DIRS}"
	"${strusbase_LIBRARY_DIRS}"
	"${strusmodule_LIBRARY_DIRS}"
	"${strusrpc_LIBRARY_DIRS}"
)


# ------------------------------
# LIBRARIES
# ------------------------------
add_library( strus_bindings_lua_impl STATIC ${lua_source_files} )

add_library( strus_bindingsimpl STATIC ${bindingsimpl_source_files} )
target_link_libraries( strus_bindingsimpl strus_storage_objbuild strus_analyzer_objbuild strus_pattern_serialize strus_module strus_rpc_client strus_rpc_client_socket strus_base strus_error )

add_library( strus_bindings SHARED ${bindings_source_files} )
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
target_link_libraries( strusLua  "${Boost_LIBRARIES}" strus_bindings_lua_impl strus_base strus_error strus_module strus_rpc_client strus_rpc_client_socket ${Intl_LIBRARIES} )

add_executable( strusBindingsModuleGen strusBindingsModuleGen.cpp )
target_link_libraries( strusBindingsModuleGen  "${Boost_LIBRARIES}" strus_base papuga_lua_gen strus_bindings_description ${Intl_LIBRARIES} )

# FreeBSD needs kernel data access library for libuv (-libkvm)
find_library( LIBKVM_LIBRARIES kvm )
if(LIBKVM_LIBRARIES)
        target_link_libraries( strusLua ${LIBKVM_LIBRARIES} )
endif()

# ------------------------------
# INSTALLATION
# ------------------------------
install( TARGETS strus_bindings
           LIBRARY DESTINATION ${LIB_INSTALL_DIR}/strus
           RUNTIME DESTINATION bin )

install( TARGETS strusLua
           RUNTIME DESTINATION bin )

