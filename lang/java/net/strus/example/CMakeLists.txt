cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

set( CMAKE_SWIG_FLAGS -package net.strus.example )
set( CMAKE_JAVA_TARGET_VERSION ${STRUS_VERSION} )

FUNCTION( build_example NAME )
	add_custom_command(
		OUTPUT ${NAME}.class
		DEPENDS ${NAME}.java
		COMMAND ${Java_JAVAC_EXECUTABLE} ARGS -classpath ${PROJECT_SOURCE_DIR}/lang/java ${NAME}.java
)
ENDFUNCTION( build_example CNAM )

set( tests HelloWorld CreateCollection CreateCollectionNoAnalyzer Status QueryNoAnalyzer )
foreach( test ${tests} )
	build_example( ${test} )
endforeach( test )

add_custom_target( run ALL DEPENDS HelloWorld.class CreateCollection.class CreateCollectionNoAnalyzer.class Status.class QueryNoAnalyzer.class )

set( THISDIR "${PROJECT_SOURCE_DIR}/lang/java/net/strus/example" )

foreach( test ${tests} )
	add_test( NAME java_${test} COMMAND java -classpath ../../.. net.strus.example.${test} WORKING_DIRECTORY ${THISDIR} )
	set_tests_properties( java_${test} PROPERTIES ENVIRONMENT "LD_LIBRARY_PATH=${PROJECT_SOURCE_DIR}/lang/java" )
endforeach( test )

