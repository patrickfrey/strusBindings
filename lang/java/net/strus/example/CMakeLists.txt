cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

set( CMAKE_SWIG_FLAGS -package net.strus.example )
set( CMAKE_JAVA_TARGET_VERSION ${STRUS_VERSION} )

FUNCTION( build_example NAME )
	add_custom_command(
		OUTPUT ${NAME}.class
		DEPENDS ${NAME}.java
		COMMAND ${Java_JAVAC_EXECUTABLE} ARGS -classpath ${PROJECT_SOURCE_DIR}/lang/java ${NAME}.java
)
ENDFUNCTION( build_example CNAM )

set( tests HelloWorld CreateCollection CreateCollectionNoAnalyzer Status QueryNoAnalyzer QueryWithAnalyzer QueryWithAnalyzer_trace )
foreach( test ${tests} )
	build_example( ${test} )
endforeach( test )

add_custom_target( run ALL DEPENDS HelloWorld.class CreateCollection.class CreateCollectionNoAnalyzer.class Status.class QueryNoAnalyzer.class QueryWithAnalyzer.class QueryWithAnalyzer_trace.class )

set( THISDIR "${PROJECT_SOURCE_DIR}/lang/java/net/strus/example" )

foreach( test ${tests} )
	if( APPLE OR WIN32 )
		set( JNI_NATIVE_CODE_DIR "${PROJECT_SOURCE_DIR}/lang/java/${CMAKE_BUILD_TYPE}" )
	else( APPLE OR WIN32 )
		set( JNI_NATIVE_CODE_DIR "${PROJECT_SOURCE_DIR}/lang/java" )
	endif( APPLE OR WIN32 )
	add_test( NAME java_${test} COMMAND java -Djava.library.path=${JNI_NATIVE_CODE_DIR} -classpath ../../.. net.strus.example.${test} WORKING_DIRECTORY ${THISDIR} )
#	set_tests_properties( java_${test} PROPERTIES ENVIRONMENT "LD_LIBRARY_PATH=${JNI_NATIVE_CODE_DIR}" )
endforeach( test )

