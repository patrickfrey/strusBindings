cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

configure_file( "php.ini.in"  "${CMAKE_CURRENT_BINARY_DIR}/php.ini"  @ONLY )

set( PHP7_TESTDIR "${PROJECT_SOURCE_DIR}/tests/php7" )
set( PHP7_DATADIR "${PROJECT_SOURCE_DIR}/tests/data" )
set( PHP7_EXECDIR "${CMAKE_CURRENT_BINARY_DIR}/exec" )
add_custom_target( Php7TestExecDir ALL COMMAND ${CMAKE_COMMAND} -E make_directory "${PHP7_EXECDIR}" )

set( TEST_LD_LIBRARY_PATH "${CMAKE_CURRENT_BINARY_DIR}/../../3rdParty/papuga/php7" )
set( TEST_LD_LIBRARY_PATH "${TEST_LD_LIBRARY_PATH}:${CMAKE_CURRENT_BINARY_DIR}/../../src/bindings" )
set( TEST_LD_LIBRARY_PATH "${TEST_LD_LIBRARY_PATH}:${Boost_LIBRARY_DIRS}" )

macro(add_php7_test name args )
    add_test( Php7${name} ${PHP7_EXECUTABLE} -c ${CMAKE_CURRENT_BINARY_DIR}/php.ini -f "${PHP7_TESTDIR}/test${name}.php" ${args} )
    set_property( TEST Php7${name}  APPEND PROPERTY ENVIRONMENT LD_LIBRARY_PATH=${TEST_LD_LIBRARY_PATH} )  
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





