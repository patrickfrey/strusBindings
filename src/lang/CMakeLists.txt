cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

file( MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/include/strus/bindings )

# Subdirectories:
add_subdirectory( lua )
add_subdirectory( php7 )
