cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

# --------------------------------------
# SOURCES AND INCLUDES
# --------------------------------------
set( bindingsimpl_source_files
	expressionBuilder.cpp
	patternMatcherLoader.cpp
	structNameMap.cpp
	structDefs.cpp
	metadataop.cpp
	serializer.cpp
	deserializer.cpp
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
set_source_files_properties( bindingObjects.cpp PROPERTIES GENERATED TRUE ) 
set_source_files_properties( libstrus_bindings_description.cpp PROPERTIES GENERATED TRUE ) 

include_directories(
	"${Boost_INCLUDE_DIRS}"
	"${Intl_INCLUDE_DIRS}"
	"${PROJECT_SOURCE_DIR}/3rdParty/lua"
	"${PROJECT_SOURCE_DIR}/3rdParty/textwolf/include"
	"${PROJECT_SOURCE_DIR}/3rdParty/papuga/include"
	"${CMAKE_BINARY_DIR}/include"
	"${PROJECT_SOURCE_DIR}/include"
	"${CMAKE_BINARY_DIR}/src/bindings"
	"${PROJECT_SOURCE_DIR}/src/bindings"
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
target_link_libraries( strus_bindings strus_bindingsimpl strus_base strus_error papuga_devel )

add_library( strus_bindings_description SHARED libstrus_bindings_description.cpp )
add_dependencies( strus_bindings_description  strus_bindings_c_interface )
target_link_libraries( strus_bindings_description )

set_target_properties(
    strus_bindings
    PROPERTIES
    DEBUG_POSTFIX "_d"
    SOVERSION "${STRUS_MAJOR_VERSION}.${STRUS_MINOR_VERSION}"
    VERSION ${STRUS_VERSION}
)

# ------------------------------
# INSTALLATION
# ------------------------------
install( TARGETS strus_bindings
           LIBRARY DESTINATION ${LIB_INSTALL_DIR}/strus
           RUNTIME DESTINATION bin )


