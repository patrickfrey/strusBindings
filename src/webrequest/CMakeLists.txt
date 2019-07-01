cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

if( NOT STRUS_CXX_STD_11 )
MESSAGE( ERROR "Compiling with C++ version lower than 11, the webrequest bindings can not be built" )
endif( NOT STRUS_CXX_STD_11 )

# --------------------------------------
# SOURCES AND INCLUDES
# --------------------------------------
set( source_files
	configurationHandler.cpp
	configurationUpdateRequest.cpp
	transaction.cpp
	curlEventLoop.cpp
	webRequestUtils.cpp
	webRequestDelegateConnection.cpp
	webRequestHandler.cpp
	webRequestContext.cpp
)

include_directories(
	${Boost_INCLUDE_DIRS}
	"${Intl_INCLUDE_DIRS}"
	${BINDINGS_INCLUDE_DIRS}
	${BINDINGS_SOURCE_DIRS}
	${CMAKE_CURRENT_BINARY_DIR}
	${PAPUGA_INCLUDE_DIRS}
	${CURL_INCLUDE_DIRS}
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
add_cppcheck( strus_webrequest ${source_files}  libstrus_webrequest.cpp )

add_library( strus_webrequest_static STATIC ${source_files} )
target_link_libraries( strus_webrequest_static strus_bindings strus_bindings_description strusbindings_private_utils strus_base papuga_request_devel papuga_devel ${CURL_LIBRARIES} )

add_library( strus_webrequest SHARED  ${source_files} libstrus_webrequest.cpp )
target_link_libraries( strus_webrequest strus_bindings strus_bindings_description strusbindings_private_utils strus_base papuga_request_devel papuga_devel ${CURL_LIBRARIES} )

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

