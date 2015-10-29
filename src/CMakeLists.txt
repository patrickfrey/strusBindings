cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

# --------------------------------------
# SOURCES AND INCLUDES
# --------------------------------------
set( source_files
	bindingObjects.cpp
	utils.cpp
	wcharString.cpp
	internationalization.cpp
)

include_directories(
	"${PROJECT_SOURCE_DIR}/include"
	"${strus_INCLUDE_DIRS}"
	"${strusanalyzer_INCLUDE_DIRS}"
	"${strusmodule_INCLUDE_DIRS}"
	"${strusrpc_INCLUDE_DIRS}"
)

link_directories(
	"${strusmodule_LIBRARY_DIRS}"
	"${strusrpc_LIBRARY_DIRS}"
)


add_library( strus_bindings STATIC ${source_files} )
target_link_libraries( strus_bindings strus_module strus_rpc_client strus_rpc_client_socket strus_utils strus_error )


