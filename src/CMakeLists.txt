cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

# Subdirectories:
add_subdirectory( utils )
add_subdirectory( parser )
add_subdirectory( gen )
add_subdirectory( bindings )
add_subdirectory( lang )
if( STRUS_CXX_STD_11 )
add_subdirectory( webrequest )
else( STRUS_CXX_STD_11 )
MESSAGE( STATUS "Compiling with C++ version lower than 11, the webrequest bindings will not be built" )
endif( STRUS_CXX_STD_11 )
