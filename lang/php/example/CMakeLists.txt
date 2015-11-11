cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

set( THISDIR "${PROJECT_SOURCE_DIR}/lang/php/example" )
set( tests helloWorld createCollection query queryexpr status )

foreach( test ${tests} )
	add_test( NAME php_${test} COMMAND php ${test}.php WORKING_DIRECTORY ${THISDIR} )
	set_tests_properties( php_${test} PROPERTIES ENVIRONMENT "LD_LIBRARY_PATH=${PROJECT_SOURCE_DIR}/lang/php" )
endforeach( test )

