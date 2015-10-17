cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

# --------------------------------------
# SOURCES AND INCLUDES
# --------------------------------------
set( source_files
	testQuery.cpp
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

# --------------------------------------
# EXECUTABLES
# --------------------------------------
add_executable( testQuery ${source_files} )
#target_link_libraries( testQuery strus_bindings )
target_link_libraries( testQuery strus_bindings strus_module strus_rpc_client strus_rpc_client_socket strus_utils strus_error )

