cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

set( PY3_TESTDIR "${PROJECT_SOURCE_DIR}/tests/python3" )
set( PY3_DATADIR "${PROJECT_SOURCE_DIR}/tests/data" )
set( PY3_EXECDIR "${CMAKE_CURRENT_BINARY_DIR}/exec" )
add_custom_target( Python3TestExecDir ALL COMMAND ${CMAKE_COMMAND} -E make_directory "${PY3_EXECDIR}" )

macro(add_python3_test name args )
    add_test( Python3${name} ${PYTHON3_EXECUTABLE}  "${PY3_TESTDIR}/test${name}.py" ${args} )
    set_property( TEST Python3${name} PROPERTY ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}/../../3rdParty/papuga/python3:${CMAKE_CURRENT_BINARY_DIR}/../../src/bindings:$ENV{LD_LIBRARY_PATH}")
endmacro(add_python3_test)

add_python3_test( LoadStrusExtension "" )
add_python3_test( CreateCollection_t3s  "${PY3_DATADIR}/t3s"  "${PY3_EXECDIR}" )
add_python3_test( Query_t3s "${PY3_DATADIR}/t3s"  "${PY3_EXECDIR}" )
add_python3_test( CreateCollection_mdprim "${PY3_DATADIR}/mdprim" "${PY3_EXECDIR}" )
add_python3_test( Query_mdprim "${PY3_DATADIR}/mdprim"  "${PY3_EXECDIR}" )
IF (WITH_STRUS_VECTOR STREQUAL "YES")
add_python3_test( Vectors "${PY3_EXECDIR}" )
ENDIF (WITH_STRUS_VECTOR STREQUAL "YES")


