cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

set( CMAKE_SWIG_FLAGS -package net.strus.example )
set( CMAKE_JAVA_TARGET_VERSION ${STRUS_VERSION} )

FUNCTION( build_example NAME )
	add_custom_command(
		OUTPUT ${NAME}.class
		DEPENDS ${NAME}.java
		COMMAND ${Java_JAVAC_EXECUTABLE} ARGS -classpath ${PROJECT_SOURCE_DIR}/swig/java ${NAME}.java
)
ENDFUNCTION( build_example CNAM )

build_example( HelloWorld )
build_example( CreateCollection )
build_example( CreateCollectionNoAnalyzer )
build_example( Status )

add_custom_target( run ALL DEPENDS HelloWorld.class CreateCollection.class CreateCollectionNoAnalyzer.class Status.class )

set( THISDIR "${PROJECT_SOURCE_DIR}/swig/java/net/strus/example" )

add_test( NAME HelloWorld COMMAND java -classpath ../../.. net.strus.example.HelloWorld WORKING_DIRECTORY ${THISDIR} )
add_test( NAME CreateCollection COMMAND java -classpath ../../.. net.strus.example.CreateCollection WORKING_DIRECTORY  ${THISDIR} )
add_test( NAME CreateCollectionNoAnalyzer COMMAND java -classpath ../../.. net.strus.example.CreateCollectionNoAnalyzer WORKING_DIRECTORY  ${THISDIR} )
add_test( NAME Status COMMAND java -classpath ../../.. net.strus.example.Status WORKING_DIRECTORY ${THISDIR} )

set_tests_properties( Status PROPERTIES ENVIRONMENT "LD_LIBRARY_PATH=${PROJECT_SOURCE_DIR}/swig/java" )
set_tests_properties( CreateCollection PROPERTIES ENVIRONMENT "LD_LIBRARY_PATH=${PROJECT_SOURCE_DIR}/swig/java" )
set_tests_properties( CreateCollectionNoAnalyzer PROPERTIES ENVIRONMENT "LD_LIBRARY_PATH=${PROJECT_SOURCE_DIR}/swig/java" )



