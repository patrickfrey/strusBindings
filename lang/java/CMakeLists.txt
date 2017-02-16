cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-strict-aliasing" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing" )

include_directories( "${PROJECT_SOURCE_DIR}/include"  ${JNI_INCLUDE_DIRS} ${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2} "${Boost_INCLUDE_DIRS}")
link_directories( "${PROJECT_SOURCE_DIR}/src" "${strusmodule_LIBRARY_DIRS}" "${strusrpc_LIBRARY_DIRS}" )

# Java:
add_custom_command(
	OUTPUT ${PROJECT_BINARY_DIR}/lang/java/strus_wrap.cxx
	PRE_BUILD
	COMMAND  ${SWIG_EXECUTABLE} -Werror -java -package net.strus.api -c++ -outdir ${PROJECT_BINARY_DIR}/lang/java/net/strus/api -o ${PROJECT_BINARY_DIR}/lang/java/strus_wrap.cxx strus.i
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/lang/java"
	COMMENT "build Java bindings" VERBATIM
	DEPENDS  strus.i exception_handler.i  ignore_declarations.i  java_heap.i
)

add_library( strus_java SHARED strus_wrap.cxx objInitializers.cpp )
target_link_libraries( strus_java  strus_bindings )

set_target_properties(
	strus_java
	PROPERTIES
	DEBUG_POSTFIX "_d"
	SOVERSION "${STRUS_MAJOR_VERSION}.${STRUS_MINOR_VERSION}"
 	VERSION ${STRUS_VERSION}
)
target_link_libraries( strus_java  strus_bindings ${JNI_LIBRARIES} ${JAVA_MAWT_LIBRARY} ${Intl_LIBRARIES} )

add_subdirectory( net )

# ------------------------------
# INSTALLATION
# ------------------------------
install( TARGETS strus_java
           LIBRARY DESTINATION ${LIB_INSTALL_DIR}/strus )



