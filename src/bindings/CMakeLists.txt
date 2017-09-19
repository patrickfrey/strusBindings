cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

# --------------------------------------
# SOURCES AND INCLUDES
# --------------------------------------
set( bindings_source_files
	expressionBuilder.cpp
	patternMatcherLoader.cpp
	structNameMap.cpp
	structDefs.cpp
	serializer.cpp
	deserializer.cpp
	valueVariantWrap.cpp
	callResultUtils.cpp
	utils.cpp
	traceUtils.cpp
	${CMAKE_CURRENT_BINARY_DIR}/internationalization.cpp
	impl/value/termExpression.cpp
	impl/value/metadataExpression.cpp
	impl/value/postingIterator.cpp
	impl/value/valueIterator.cpp
	impl/value/selectIterator.cpp
	impl/value/statisticsIterator.cpp
	impl/value/forwardTermsIterator.cpp
	impl/value/searchTermsIterator.cpp
	impl/analyzer.cpp
	impl/query.cpp
	impl/storage.cpp
	impl/vector.cpp
	impl/context.cpp
	${CMAKE_CURRENT_BINARY_DIR}/bindingObjects.cpp
)
set_source_files_properties( ${CMAKE_CURRENT_BINARY_DIR}/bindingObjects.cpp PROPERTIES GENERATED TRUE ) 
set_source_files_properties( ${CMAKE_CURRENT_BINARY_DIR}/libstrus_bindings_description.cpp PROPERTIES GENERATED TRUE ) 
set_source_files_properties( ${CMAKE_CURRENT_BINARY_DIR}/../../include/strus/lib/bindingObjects.h PROPERTIES GENERATED TRUE ) 
set_source_files_properties( ${CMAKE_CURRENT_BINARY_DIR}/../../include/strus/lib/bindings_description.hpp PROPERTIES GENERATED TRUE ) 
set_source_files_properties( ${CMAKE_CURRENT_BINARY_DIR}/bindingClassIds.hpp PROPERTIES GENERATED TRUE ) 
set_source_files_properties( ${CMAKE_CURRENT_BINARY_DIR}/bindingClassTemplate.hpp PROPERTIES GENERATED TRUE ) 

include_directories(
	${Boost_INCLUDE_DIRS}
	${Intl_INCLUDE_DIRS}
	${LUA_INCLUDE_DIRS}
	${TEXTWOLF_INCLUDE_DIRS}
	${PAPUGA_INCLUDE_DIRS}
	${BINDINGS_INCLUDE_DIRS}
	${BINDINGS_SOURCE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
	${strusbase_INCLUDE_DIRS}
	${strus_INCLUDE_DIRS}
	${strusanalyzer_INCLUDE_DIRS}
	${strusmodule_INCLUDE_DIRS}
	${strustrace_INCLUDE_DIRS}
	${strusrpc_INCLUDE_DIRS}
)

link_directories(
	${LUA_LIBRARY_DIRS}
	${PAPUGA_LIBRARY_DIRS}
	${BINDINGS_LIBRARY_DIRS}
	${Boost_LIBRARY_DIRS}
	${strusbase_LIBRARY_DIRS}
	${strusmodule_LIBRARY_DIRS}
	${strusrpc_LIBRARY_DIRS}
)

# ------------------------------
# LIBRARIES
# ------------------------------
add_library( strus_bindings SHARED ${bindings_source_files} )
add_dependencies( strus_bindings  strus_bindings_c_interface )
target_link_libraries( strus_bindings strus_base strus_error papuga_devel strus_storage_objbuild strus_analyzer_objbuild strus_pattern_serialize strus_module strus_rpc_client strus_rpc_client_socket )

add_library( strus_bindings_description SHARED ${CMAKE_CURRENT_BINARY_DIR}/libstrus_bindings_description.cpp )
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


