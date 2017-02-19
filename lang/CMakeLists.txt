cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

IF (WITH_JAVA STREQUAL "YES")
add_subdirectory( java )
ENDIF (WITH_JAVA STREQUAL "YES")

IF (WITH_PHP STREQUAL "YES")
add_subdirectory( php )
ENDIF (WITH_PHP STREQUAL "YES")

IF (WITH_PYTHON STREQUAL "YES")
add_subdirectory( python )
ENDIF (WITH_PYTHON STREQUAL "YES")



