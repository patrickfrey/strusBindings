cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

# Subdirectories:
add_subdirectory( src )

set( ENV{LD_LIBRARY_PATH} "$(TOPDIR)/src:$(INSTALL_LIBDIR)" )
add_test( QueryCpp src/testBindingsQuery )


