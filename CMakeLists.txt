cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

# must be done BEFORE project in order to work!
include(cmake/install_prefix.cmake)

# Project globals:
project( StrusBindings )
set( STRUS_MAJOR_VERSION 0 )
set( STRUS_MINOR_VERSION 15 )
set( STRUS_PATCH_VERSION 0 )
set( STRUS_VERSION ${STRUS_MAJOR_VERSION}.${STRUS_MINOR_VERSION}.${STRUS_PATCH_VERSION} )

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)

IF (WITH_PHP STREQUAL "YES")
MESSAGE( STATUS  "PHP (v7)  bindings generation is enabled." )
ELSE (WITH_PHP STREQUAL "YES")
MESSAGE( STATUS  "PHP bindings generation is not enabled. Start cmake with -DWITH_PHP=YES to enable it." )
ENDIF (WITH_PHP STREQUAL "YES")
    
IF (WITH_PYTHON STREQUAL "YES")
MESSAGE( STATUS  "Python (v3) bindings generation is enabled." )
ELSE (WITH_PYTHON STREQUAL "YES")
MESSAGE( STATUS  "Python bindings generation is not enabled. Start cmake with -DWITH_PYTHON=YES to enable it." )
ENDIF (WITH_PYTHON STREQUAL "YES")
    
set( CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" )
if(NOT STRUS_ALL)
include( cmake/dist.cmake )
include( cmake/LinuxDistribution.cmake )
include( cmake/build_rules.cmake )
include( cmake/boost.cmake)
include( cmake/intl.cmake )
include( cmake/standard_targets.cmake )
endif()

IF (WITH_STRUS_VECTOR STREQUAL "YES")
MESSAGE( STATUS  "Tests for strusVector enabled." )
ENDIF (WITH_STRUS_VECTOR STREQUAL "YES")
IF (WITH_STRUS_PATTERN STREQUAL "YES")
MESSAGE( STATUS  "Tests for strusPattern enabled." )
ENDIF (WITH_STRUS_PATTERN STREQUAL "YES")

if(NOT STRUS_ALL)
include( cmake/find_strus_package.cmake )
endif()

find_strus_package( base )
find_strus_package( core )
find_strus_package( analyzer )
find_strus_package( module )
find_strus_package( trace )
find_strus_package( rpc )

configure_file( "${PROJECT_SOURCE_DIR}/include/strus/versionBindings.hpp.in"   "${CMAKE_CURRENT_BINARY_DIR}/include/strus/versionBindings.hpp"   @ONLY )

set( STRUS_GETTEXT_PACKAGE "strus-dom" )	#... 1st parameter of bindtextdomain(...)
set( STRUS_GETTEXT_LOCALEDIR "" )			#... 2nd parameter of bindtextdomain(...)
configure_file( "${PROJECT_SOURCE_DIR}/src/bindings/internationalization.cpp.in"  "${CMAKE_CURRENT_BINARY_DIR}/src/bindings/internationalization.cpp"  @ONLY )
IF (WITH_PHP STREQUAL "YES")
configure_file( "${PROJECT_SOURCE_DIR}/tests/php7/php.ini.in"  "${CMAKE_CURRENT_BINARY_DIR}/tests/php7/php.ini"  @ONLY )
ENDIF (WITH_PHP STREQUAL "YES")
configure_file( "${PROJECT_SOURCE_DIR}/dist/obs/deploy_to_obs.sh.in"  "${PROJECT_SOURCE_DIR}/dist/obs/deploy_to_obs.sh"  @ONLY )
configure_file( "${PROJECT_SOURCE_DIR}/dist/archlinux/PKGBUILD.in"  "${PROJECT_SOURCE_DIR}/dist/archlinux/PKGBUILD"  @ONLY )
configure_file( "${PROJECT_SOURCE_DIR}/dist/obs/strusbindings.dsc.in"  "${PROJECT_SOURCE_DIR}/dist/obs/strusbindings.dsc"  @ONLY )
configure_file( "${PROJECT_SOURCE_DIR}/dist/obs/strusbindings-xUbuntu_14.04.dsc.in"  "${PROJECT_SOURCE_DIR}/dist/obs/strusbindings-xUbuntu_14.04.dsc"  @ONLY )
configure_file( "${PROJECT_SOURCE_DIR}/dist/obs/strusbindings-Debian_8.0.dsc.in"  "${PROJECT_SOURCE_DIR}/dist/obs/strusbindings-Debian_8.0.dsc"  @ONLY )
configure_file( "${PROJECT_SOURCE_DIR}/dist/redhat/strusbindings.spec.in"  "${PROJECT_SOURCE_DIR}/dist/redhat/strusbindings.spec"  @ONLY )

enable_testing()

# Subdirectories:
add_subdirectory( 3rdParty )
add_subdirectory( src )
add_subdirectory( tests )
  
if(NOT STRUS_ALL)
include( cmake/report_build_settings.cmake )
endif()

# Install header files:
install(DIRECTORY include/strus
          DESTINATION include
          FILES_MATCHING PATTERN "*.hpp")
install(DIRECTORY include/strus
          DESTINATION include
          FILES_MATCHING PATTERN "*.h")
install(DIRECTORY  3rdParty/papuga/include/papuga
          DESTINATION include
          FILES_MATCHING PATTERN "*.h")
install(FILES  3rdParty/papuga/include/papuga.h
          DESTINATION include )
# Install generated header files:
install(DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/include/strus
          DESTINATION include
          FILES_MATCHING PATTERN "*.hpp")
install(DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/include/strus
          DESTINATION include
          FILES_MATCHING PATTERN "*.h")


