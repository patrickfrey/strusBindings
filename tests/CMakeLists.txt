cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

# Subdirectories:
add_subdirectory( request_parse )
add_subdirectory( lua )
IF (WITH_PHP STREQUAL "YES")
add_subdirectory( php7 )
ENDIF (WITH_PHP STREQUAL "YES")
IF (WITH_PYTHON STREQUAL "YES")
add_subdirectory( python3 )
ENDIF (WITH_PYTHON STREQUAL "YES")


