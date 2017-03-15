cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

# --------------------------------------
# SOURCES AND INCLUDES
# --------------------------------------
# Subdirectories:
add_subdirectory( parser )
add_subdirectory( filter )

set( source_files
	testcheck.cpp
	bindingObjects.cpp
	utils.cpp
	traceUtils.cpp
	wcharString.cpp
	internationalization.cpp
)

include_directories(
	"${Boost_INCLUDE_DIRS}"
	"${Intl_INCLUDE_DIRS}"
	"${PROJECT_SOURCE_DIR}/3rdParty/textwolf/include"
	"${PROJECT_SOURCE_DIR}/include"
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


add_library( strus_bindings STATIC ${source_files} )
target_link_libraries( strus_bindings strus_storage_objbuild strus_pattern_serialize strus_module strus_rpc_client strus_rpc_client_socket strus_base strus_error )


