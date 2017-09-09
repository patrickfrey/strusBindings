cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

add_custom_target( Python3TestExecDir ALL COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/exec" )
set( PY3_TESTDIR "${PROJECT_SOURCE_DIR}/tests/python3" )
set( PY3_DATADIR "${PROJECT_SOURCE_DIR}/tests/data" )
set( PY3_EXECDIR "${CMAKE_CURRENT_BINARY_DIR}/exec" )

add_test( Python3LoadStrusExtension ${PYTHON3_EXECUTABLE}  "${PY3_TESTDIR}/testLoadStrusExtension.py" )
add_test( Python3CreateCollection_t3s ${PYTHON3_EXECUTABLE}  "${PY3_TESTDIR}/testCreateCollection_t3s.py"  "${PY3_DATADIR}/t3s"  "${PY3_EXECDIR}" )
add_test( Python3Query_t3s ${PYTHON3_EXECUTABLE}  "${PY3_TESTDIR}/testQuery_t3s.py"  "${PY3_DATADIR}/t3s"  "${PY3_EXECDIR}" )
add_test( Python3CreateCollection_mdprim ${PYTHON3_EXECUTABLE}  "${PY3_TESTDIR}/testCreateCollection_mdprim.py"  "${PY3_DATADIR}/mdprim" "${PY3_EXECDIR}" )
add_test( Python3Query_mdprim ${PYTHON3_EXECUTABLE}  "${PY3_TESTDIR}/testQuery_mdprim.py"  "${PY3_DATADIR}/mdprim"  "${PY3_EXECDIR}" )
add_test( Python3Vectors ${PYTHON3_EXECUTABLE}  "${PY3_TESTDIR}/testVectors.py"  "${PY3_EXECDIR}" )

