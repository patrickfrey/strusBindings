cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-strict-aliasing" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing" )

include_directories( "${PROJECT_SOURCE_DIR}/include"  ${JNI_INCLUDE_DIRS} ${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2} )
link_directories( "${PROJECT_SOURCE_DIR}/src" "${strusmodule_LIBRARY_DIRS}" "${strusrpc_LIBRARY_DIRS}" )

# Java:
add_custom_command(
	OUTPUT strus_wrap.cxx java_strus.h
	PRE_BUILD
	COMMAND  swig -Werror -java -package net.strus.api -c++ -outdir ${PROJECT_SOURCE_DIR}/swig/java/net/strus/api -o strus_wrap.cxx strus.i
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/swig/java"
	COMMENT "build Java bindings" VERBATIM
	DEPENDS  strus.i strus_variant.i strus_tokenizer.i strus_normalizer.i strus_aggregator.i std_vector_string.i std_vector_rank.i std_vector_term.i exception_handler.i   strus_weighting.i strus_summarizer.i ignore_declarations.i
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
target_link_libraries( strus_java  strus_bindings ${JNI_LIBRARIES} )

add_subdirectory( net )

# ------------------------------
# INSTALLATION
# ------------------------------
install( TARGETS strus_java
           LIBRARY DESTINATION ${LIB_INSTALL_DIR}/strus )



