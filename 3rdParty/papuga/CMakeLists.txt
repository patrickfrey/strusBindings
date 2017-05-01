cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

# must be done BEFORE project in order to work!
# include(cmake/install_prefix.cmake)

# Project globals:
# project( papuga )

set( PAPUGA_MAJOR_VERSION 0 )
set( PAPUGA_MINOR_VERSION 0 )
set( PAPUGA_PATCH_VERSION 1 )
set( PAPUGA_VERSION ${PAPUGA_MAJOR_VERSION}.${PAPUGA_MINOR_VERSION}.${PAPUGA_PATCH_VERSION} )

# include( cmake/dist.cmake )
# include( cmake/LinuxDistribution.cmake )
# include( cmake/build_rules.cmake )
# include( cmake/intl.cmake )

configure_file( "${PROJECT_SOURCE_DIR}/3rdParty/papuga/include/papuga/version.h.in"   "${PROJECT_SOURCE_DIR}/3rdParty/papuga/include/papuga/version.h"   @ONLY )

# Subdirectories:
add_subdirectory( src )
add_subdirectory( lua )
  
# include( cmake/report_build_settings.cmake )
# include( cmake/standard_targets.cmake )


