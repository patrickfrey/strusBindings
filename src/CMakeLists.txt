cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

# --------------------------------------
# SOURCES AND INCLUDES
# --------------------------------------
set( source_files
	bindingObjects.cpp
	utils.cpp
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


add_library( strus_bindings SHARED ${source_files} )
target_link_libraries( strus_bindings "${strusmodule_LIBRARIES}" "${strusrpc_LIBRARIES}" )
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
           LIBRARY DESTINATION ${LIB_INSTALL_DIR}/strus )

