cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

# --------------------------------------
# SOURCES AND INCLUDES
# --------------------------------------
# Subdirectories:
add_subdirectory( parser )

set( bindingsimpl_source_files
        valueVariantConv.cpp
	expressionBuilder.cpp
	patternMatcherLoader.cpp
	structNameMap.cpp
	structDefs.cpp
	serializer.cpp
	deserializer.cpp
	testcheck.cpp
	wcharString.cpp
	utils.cpp
	traceUtils.cpp
	internationalization.cpp
	impl/analyzer.cpp
	impl/query.cpp
	impl/storage.cpp
        impl/statistics.cpp
	impl/vectorStorage.cpp
	impl/context.cpp
)

set( bindings_source_files
	bindingObjects.cpp
)

include_directories(
	"${Boost_INCLUDE_DIRS}"
	"${Intl_INCLUDE_DIRS}"
	"${PROJECT_SOURCE_DIR}/3rdParty/textwolf/include"
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
	"${Boost_LIBRARY_DIRS}"
	"${strusbase_LIBRARY_DIRS}"
	"${strusmodule_LIBRARY_DIRS}"
	"${strusrpc_LIBRARY_DIRS}"
)


add_library( strus_bindingsimpl STATIC ${bindingsimpl_source_files} )
target_link_libraries( strus_bindingsimpl strus_storage_objbuild strus_analyzer_objbuild strus_pattern_serialize strus_module strus_rpc_client strus_rpc_client_socket strus_base strus_error )

add_library( strus_bindings STATIC ${bindings_source_files} )
target_link_libraries( strus_bindings strus_bindingsimpl strus_base strus_error )


