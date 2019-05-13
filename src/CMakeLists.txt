cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

# Subdirectories:
add_subdirectory( utils )
add_subdirectory( parser )
add_subdirectory( gen )
add_subdirectory( bindings )
add_subdirectory( lang )
if( WITH_WEBREQUEST STREQUAL "YES" )
add_subdirectory( webrequest )
endif( WITH_WEBREQUEST STREQUAL "YES" )

