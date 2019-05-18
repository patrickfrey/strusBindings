cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

# Subdirectories:
if( WITH_WEBREQUEST STREQUAL "YES" )
add_subdirectory( request_transactionmap )
add_subdirectory( request_parse )
endif( WITH_WEBREQUEST STREQUAL "YES" )

IF (WITH_LUA STREQUAL "YES")
add_subdirectory( lua )
ENDIF (WITH_LUA STREQUAL "YES")

IF (WITH_PHP STREQUAL "YES")
add_subdirectory( php7 )
ENDIF (WITH_PHP STREQUAL "YES")

IF (WITH_PYTHON STREQUAL "YES")
add_subdirectory( python3 )
ENDIF (WITH_PYTHON STREQUAL "YES")


