cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

# Subdirectories:
if( WITH_WEBREQUEST STREQUAL "YES" )
add_subdirectory( request_transactionmap )
add_subdirectory( request_parse )
add_subdirectory( schemaid )
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

if( WITH_WEBREQUEST STREQUAL "YES" AND WITH_LUA STREQUAL "YES" )
add_subdirectory( request_luasim )
endif( WITH_WEBREQUEST STREQUAL "YES" AND WITH_LUA STREQUAL "YES" )

