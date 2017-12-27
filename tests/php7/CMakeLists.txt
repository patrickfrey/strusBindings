cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

get_filename_component( PHP7_LOCAL_EXTENSION_DIR "${CMAKE_BINARY_DIR}/modules/php7" ABSOLUTE )
get_filename_component( PHP7_LOCAL_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/tests/php7" ABSOLUTE )
get_filename_component( PHP7_LOCAL_DATA_DIR "${PROJECT_SOURCE_DIR}/tests/data" ABSOLUTE )

configure_file( "php.ini.in"  "${CMAKE_CURRENT_BINARY_DIR}/php.ini"  @ONLY )

set( PHP7_TESTDIR "${PROJECT_SOURCE_DIR}/tests/php7" )
set( PHP7_DATADIR "${PROJECT_SOURCE_DIR}/tests/data" )
set( PHP7_EXECDIR "${CMAKE_CURRENT_BINARY_DIR}/exec" )
add_custom_target( Php7TestExecDir ALL COMMAND ${CMAKE_COMMAND} -E make_directory "${PHP7_EXECDIR}" )

macro(add_php7_test name )
    add_test( Php7${name} ${PHP7_EXECUTABLE} -c ${CMAKE_CURRENT_BINARY_DIR}/php.ini -f "${PHP7_TESTDIR}/test${name}.php"  ${ARGN} )
    set_property( TEST Php7${name}  APPEND PROPERTY ENVIRONMENT LD_LIBRARY_PATH="${CMAKE_BINARY_DIR}/lib" )  
    set_property( TEST Php7${name}  APPEND PROPERTY ENVIRONMENT STRUS_MODULE_PATH=${STRUS_MODULE_PATH} )
endmacro(add_php7_test)

add_php7_test( PrintEnv "" )
add_php7_test( LoadStrusExtension "" )
add_php7_test( CreateCollection_t3s  "${PHP7_DATADIR}/t3s"  "${PHP7_EXECDIR}" )
add_php7_test( Query_t3s "${PHP7_DATADIR}/t3s"  "${PHP7_EXECDIR}" )
add_php7_test( CreateCollection_mdprim "${PHP7_DATADIR}/mdprim" "${PHP7_EXECDIR}" )
add_php7_test( Query_mdprim "${PHP7_DATADIR}/mdprim"  "${PHP7_EXECDIR}" )
IF (WITH_STRUS_VECTOR STREQUAL "YES")
add_php7_test( Vectors "${PHP7_EXECDIR}" )
ENDIF (WITH_STRUS_VECTOR STREQUAL "YES")
add_php7_test( Serialization "${PHP7_EXECDIR}" )





