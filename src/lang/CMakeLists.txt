cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../../include/strus/bindings )
file( MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/doc )
file( MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/doc/images )

configure_file( ${PROJECT_SOURCE_DIR}/doc/images/logotype.jpg  ${CMAKE_BINARY_DIR}/doc/images/logotype.jpg  COPYONLY )

# Subdirectories:
IF (WITH_LUA STREQUAL "YES")
add_subdirectory( lua )
ENDIF (WITH_LUA STREQUAL "YES")

IF (WITH_PHP STREQUAL "YES")
add_subdirectory( php7 )
ENDIF (WITH_PHP STREQUAL "YES")

IF (WITH_PYTHON STREQUAL "YES")
add_subdirectory( python3 )
ENDIF (WITH_PYTHON STREQUAL "YES")
