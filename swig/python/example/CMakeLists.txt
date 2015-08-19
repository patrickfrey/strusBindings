cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

set( THISDIR "${PROJECT_SOURCE_DIR}/swig/python/example" )
set( tests helloWorld status )

foreach( test ${tests} )
	add_test( NAME python_${test} COMMAND python ${test}.py WORKING_DIRECTORY ${THISDIR} )
	set_tests_properties( python_${test} PROPERTIES ENVIRONMENT "PYTHONPATH=${PROJECT_SOURCE_DIR}/swig/python" )
endforeach( test )

