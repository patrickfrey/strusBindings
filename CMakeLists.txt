cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

# must be done BEFORE project in order to work!
include(cmake/install_prefix.cmake)

# Project globals:
project( StrusBindings )
set( STRUS_MAJOR_VERSION 0 )
set( STRUS_MINOR_VERSION 1 )
set( STRUS_PATCH_VERSION 7 )
set( STRUS_VERSION ${STRUS_MAJOR_VERSION}.${STRUS_MINOR_VERSION}.${STRUS_PATCH_VERSION} )

include( cmake/dist.cmake )
include( cmake/LinuxDistribution.cmake )
include( cmake/build_rules.cmake )

find_package( PythonLibs QUIET )
IF (${PYTHONLIBS_FOUND} STREQUAL "TRUE")
   MESSAGE( STATUS  "Found PythonLibs package" )
ELSE (${PYTHONLIBS_FOUND} STREQUAL "TRUE")
   MESSAGE( STATUS  "Could not locate PythonLibs package: ${PYTHONLIBS_FOUND} ")
ENDIF (${PYTHONLIBS_FOUND} STREQUAL "TRUE")

find_package( JNI QUIET )
IF (${JNI_FOUND} STREQUAL "TRUE")
   MESSAGE( STATUS  "Found JNI package")
ELSE (${JNI_FOUND} STREQUAL "TRUE")
   MESSAGE( STATUS  "Could not locate JNI package: ${JNI_FOUND} ")
ENDIF (${JNI_FOUND} STREQUAL "TRUE")

include( cmake/find_strus_package.cmake )

find_strus_package( core )
find_strus_package( analyzer )
find_strus_package( module )
find_strus_package( rpc )

find_package( Boost 1.53.0 COMPONENTS thread system date_time )
set( CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" )
set( CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON )
configure_file( "${PROJECT_SOURCE_DIR}/src/version.hpp.in"  "${PROJECT_SOURCE_DIR}/src/version.hpp"  @ONLY )

enable_testing()

# Subdirectories:
add_subdirectory( src )
add_subdirectory( swig )
  
include( cmake/report_build_settings.cmake )

# Install header files:
install(DIRECTORY include/strus
          DESTINATION include
          FILES_MATCHING PATTERN "*.hpp")

include( cmake/standard_targets.cmake )

