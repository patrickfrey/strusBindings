cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

set( PY3_TESTDIR "${PROJECT_SOURCE_DIR}/tests/python3" )
set( PY3_DATADIR "${PROJECT_SOURCE_DIR}/tests/data" )
set( PY3_EXECDIR "${CMAKE_CURRENT_BINARY_DIR}/exec" )
add_custom_target( Python3TestExecDir ALL COMMAND ${CMAKE_COMMAND} -E make_directory "${PY3_EXECDIR}" )

set( TEST_LD_LIBRARY_PATH "${CMAKE_CURRENT_BINARY_DIR}/../../3rdParty/papuga/python3" )
set( TEST_LD_LIBRARY_PATH "${TEST_LD_LIBRARY_PATH}:${CMAKE_CURRENT_BINARY_DIR}/../../src/bindings" )
set( TEST_LD_LIBRARY_PATH "${TEST_LD_LIBRARY_PATH}:${Boost_LIBRARY_DIRS}" )

macro(add_python3_test name )
    add_test( Python3${name} ${PYTHON3_EXECUTABLE}  "${PY3_TESTDIR}/test${name}.py"  ${ARGN} )
    set_property( TEST Python3${name}  APPEND PROPERTY ENVIRONMENT LD_LIBRARY_PATH=${TEST_LD_LIBRARY_PATH} )  
    set_property( TEST Python3${name}  APPEND PROPERTY ENVIRONMENT STRUS_MODULE_PATH=${STRUS_MODULE_PATH} )
    set_property( TEST Python3${name}  APPEND PROPERTY ENVIRONMENT PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}/../../src/lang/python3 )
    set_property( TEST Python3${name}  APPEND PROPERTY ENVIRONMENT PYTHONHASHSEED=1 )
endmacro(add_python3_test)

add_python3_test( PrintEnv "" )
add_python3_test( LoadStrusExtension "" )
add_python3_test( CreateCollection_t3s  "${PY3_DATADIR}/t3s"  "${PY3_EXECDIR}" )
add_python3_test( Query_t3s "${PY3_DATADIR}/t3s"  "${PY3_EXECDIR}" )
add_python3_test( CreateCollection_mdprim "${PY3_DATADIR}/mdprim" "${PY3_EXECDIR}" )
add_python3_test( Query_mdprim "${PY3_DATADIR}/mdprim"  "${PY3_EXECDIR}" )
IF (WITH_STRUS_VECTOR STREQUAL "YES")
add_python3_test( Vectors "${PY3_EXECDIR}" )
add_python3_test( Serialization "${PY3_EXECDIR}" )
ENDIF (WITH_STRUS_VECTOR STREQUAL "YES")


