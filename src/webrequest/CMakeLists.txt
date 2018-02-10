cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# --------------------------------------
# SOURCES AND INCLUDES
# --------------------------------------
set( libstrus_webrequest_source_files
	webRequestHandler.cpp
	webRequestContext.cpp
	webRequestUtils.cpp
)

include_directories(
	${Boost_INCLUDE_DIRS}
	"${Intl_INCLUDE_DIRS}"
	${BINDINGS_INCLUDE_DIRS}
	${BINDINGS_SOURCE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
	${PAPUGA_INCLUDE_DIRS}
	${strusbase_INCLUDE_DIRS}
	${strus_INCLUDE_DIRS}
	${strusanalyzer_INCLUDE_DIRS}
	${strusmodule_INCLUDE_DIRS}
	${strustrace_INCLUDE_DIRS}
	${strusrpc_INCLUDE_DIRS}
)

link_directories(
	${Boost_LIBRARY_DIRS}
	${strusbase_LIBRARY_DIRS}
	${strus_LIBRARY_DIRS}
	${strusanalyzer_LIBRARY_DIRS}
	${strustrace_LIBRARY_DIRS}
	${strusrpc_LIBRARY_DIRS}
	${strusmodule_LIBRARY_DIRS}
	${UTILS_GENSOURCE_DIR}
	${PAPUGA_LIBRARY_DIRS}
)

# ------------------------------
# LIBRARY
# ------------------------------
add_library( strus_webrequest_static STATIC ${libstrus_webrequest_source_files} )
target_link_libraries( strus_webrequest_static strus_base strus_bindings strus_bindings_description strusbindings_private_utils papuga_request_devel )

add_library( strus_webrequest SHARED libstrus_webrequest.cpp )
target_link_libraries( strus_webrequest strus_webrequest_static )

set_target_properties(
    strus_webrequest
    PROPERTIES
    DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}"
    SOVERSION "${STRUS_MAJOR_VERSION}.${STRUS_MINOR_VERSION}"
    VERSION ${STRUS_VERSION}
)

# ------------------------------
# INSTALLATION
# ------------------------------
install( TARGETS strus_webrequest
           LIBRARY DESTINATION ${LIB_INSTALL_DIR}/strus
	   RUNTIME DESTINATION bin )

