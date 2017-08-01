cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

# --------------------------------------
# PHP
# --------------------------------------
include( php.cmake )

include_directories(
        "${PHP_INCLUDE_DIRS}"
	"${Intl_INCLUDE_DIRS}"
	"${CMAKE_BINARY_DIR}/include"
	"${PROJECT_SOURCE_DIR}/3rdParty/papuga/include"
	"${PROJECT_SOURCE_DIR}/include"
)

link_directories(
        "${PHP_LIBRARY_DIRS}"
	"${PROJECT_SOURCE_DIR}/3rdParty/lua_cmake"
	"${PROJECT_SOURCE_DIR}/3rdParty/papuga/src"
	"${PROJECT_SOURCE_DIR}/src/bindings"
	"${PROJECT_SOURCE_DIR}/src/lang/php7"
	"${strusbase_LIBRARY_DIRS}"
	"${strusmodule_LIBRARY_DIRS}"
	"${strusrpc_LIBRARY_DIRS}"
	"${strus_LIBRARY_DIRS}"
	"${strusanalyzer_LIBRARY_DIRS}"
	"${strustrace_LIBRARY_DIRS}"
)

# ------------------------------
# LIBRARIES
# ------------------------------
add_library( strus_bindings_php7 MODULE strus.c )
target_link_libraries( strus_bindings_php7 papuga_devel papuga_php7_dev strus_bindings )

set_target_properties( strus_bindings_php7 PROPERTIES OUTPUT_NAME strus PREFIX "" )

# ------------------------------
# INSTALLATION
# ------------------------------
install( TARGETS strus_bindings_php7
           LIBRARY DESTINATION "${PHP_EXTENSION_DIR}" )



