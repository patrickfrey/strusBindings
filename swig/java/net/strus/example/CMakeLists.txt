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
build_example( Status )

add_custom_target( run ALL DEPENDS HelloWorld.class Status.class )




