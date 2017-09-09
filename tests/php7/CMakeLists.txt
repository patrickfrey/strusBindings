cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

add_custom_target( Php7TestExecDir ALL COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/tests/php7/exec" )

add_test( Php7LoadStrusExtension ${PHP7_EXECUTABLE} -c ${CMAKE_BINARY_DIR}/tests/php7/php.ini -f "${PROJECT_SOURCE_DIR}/tests/php7/testLoadStrusExtension.php" )
add_test( Php7CreateCollection_t3s ${PHP7_EXECUTABLE} -c ${CMAKE_BINARY_DIR}/tests/php7/php.ini -f "${PROJECT_SOURCE_DIR}/tests/php7/testCreateCollection_t3s.php"  "${PROJECT_SOURCE_DIR}/tests/data/t3s" "${CMAKE_BINARY_DIR}/tests/php7/exec" )
add_test( Php7Query_t3s ${PHP7_EXECUTABLE} -c ${CMAKE_BINARY_DIR}/tests/php7/php.ini -f "${PROJECT_SOURCE_DIR}/tests/php7/testQuery_t3s.php"  "${PROJECT_SOURCE_DIR}/tests/data/t3s" "${CMAKE_BINARY_DIR}/tests/php7/exec" )
add_test( Php7CreateCollection_mdprim ${PHP7_EXECUTABLE} -c ${CMAKE_BINARY_DIR}/tests/php7/php.ini -f "${PROJECT_SOURCE_DIR}/tests/php7/testCreateCollection_mdprim.php"  "${PROJECT_SOURCE_DIR}/tests/data/mdprim" "${CMAKE_BINARY_DIR}/tests/php7/exec" )
add_test( Php7Query_mdprim ${PHP7_EXECUTABLE} -c ${CMAKE_BINARY_DIR}/tests/php7/php.ini -f "${PROJECT_SOURCE_DIR}/tests/php7/testQuery_mdprim.php"  "${PROJECT_SOURCE_DIR}/tests/data/mdprim" "${CMAKE_BINARY_DIR}/tests/php7/exec" )
add_test( Php7Vectors ${PHP7_EXECUTABLE} -c ${CMAKE_BINARY_DIR}/tests/php7/php.ini -f "${PROJECT_SOURCE_DIR}/tests/php7/testVectors.php"  "${CMAKE_BINARY_DIR}/tests/php7/exec" )



