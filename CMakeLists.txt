cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

# must be done BEFORE project in order to work!
include(cmake/install_prefix.cmake)

# Project globals:
project( StrusBindings )
set( STRUS_MAJOR_VERSION 0 )
set( STRUS_MINOR_VERSION 4 )
set( STRUS_PATCH_VERSION 1 )
set( STRUS_VERSION ${STRUS_MAJOR_VERSION}.${STRUS_MINOR_VERSION}.${STRUS_PATCH_VERSION} )

IF (NOT WITH_JAVA STREQUAL "YES")
MESSAGE( STATUS  "Java bindings generation is not enabled. Start cmake with -DWITH_JAVA=\"YES\" to enable it." )
ENDIF (NOT WITH_JAVA STREQUAL "YES")
IF (NOT WITH_PYTHON STREQUAL "YES")
MESSAGE( STATUS  "Python bindings generation is not enabled. Start cmake with -DWITH_PYTHON=\"YES\" to enable it." )
ENDIF (NOT WITH_PYTHON STREQUAL "YES")
IF (NOT WITH_PHP STREQUAL "YES")
MESSAGE( STATUS  "PHP bindings generation is not enabled. Start cmake with -DWITH_PHP=\"YES\" to enable it." )
ENDIF (NOT WITH_PHP STREQUAL "YES")

include( cmake/dist.cmake )
include( cmake/LinuxDistribution.cmake )
include( cmake/build_rules.cmake )
include( cmake/boost.cmake)

IF (WITH_JAVA STREQUAL "YES")
find_package( Java REQUIRED )
find_package( Java COMPONENTS Development )
include( UseJava )
IF (DEFINED Java_VERSION)
   MESSAGE( STATUS  "Found Java package ${Java_VERSION}" )
ELSE (DEFINED Java_VERSION)
   MESSAGE( STATUS  "Could not locate Java package" )
ENDIF (DEFINED Java_VERSION)

find_package( JNI REQUIRED )
IF (${JNI_FOUND} STREQUAL "TRUE")
   MESSAGE( STATUS  "Found JNI package")
ELSE (${JNI_FOUND} STREQUAL "TRUE")
   MESSAGE( STATUS  "Could not locate JNI package: ${JNI_FOUND} ")
ENDIF (${JNI_FOUND} STREQUAL "TRUE")

if(UNIX)
  # make sure we always have a JAVA_HOME variable set
  IF ("$ENV{JAVA_HOME}" STREQUAL "")
    EXECUTE_PROCESS(
      COMMAND readlink "-f" "/usr/bin/javac"
      COMMAND sed "s:/bin/javac::"
      OUTPUT_VARIABLE JAVA_HOME OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_VARIABLE JAVA_HOME_ERROR_OUTPUT
      ERROR_VARIABLE JAVA_HOME_RETURN_VALUE)
    SET( ENV{JAVA_HOME} "${JAVA_HOME}" )
  ENDIF ("$ENV{JAVA_HOME}" STREQUAL "")  
  IF ("$ENV{JAVA_HOME}" STREQUAL "" )
    MESSAGE(STATUS "JAVA_HOME_RETURN_VALUE")
    MESSAGE(FATAL_ERROR "JAVA_HOME is not set!")
  ELSE ("$ENV{JAVA_HOME}" STREQUAL "")
    SET( JAVA_HOME "$ENV{JAVA_HOME}" )
  ENDIF ("$ENV{JAVA_HOME}" STREQUAL "" )
  MESSAGE( STATUS "JAVA_HOME: ${JAVA_HOME}" )

  # avoid errors like 'libmawt.so: cannot open shared object file' (for
  # instance on Debian)
  set(JAVA_MAWT_LIBRARY "")
  find_library(JAVA_MAWT_LIBRARY NAMES mawt
    PATHS ${JAVA_HOME}/jre/lib/amd64/xawt
          ${JAVA_HOME}/jre/lib/i386/xawt
  )
endif()

ENDIF (WITH_JAVA STREQUAL "YES")

IF (WITH_PYTHON STREQUAL "YES")
	find_package( PythonLibs 2.7 REQUIRED )
	execute_process ( COMMAND /usr/bin/env python2 -c "from distutils.sysconfig import get_python_lib; print get_python_lib(1)" OUTPUT_VARIABLE PYTHON_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE)
IF (${PYTHONLIBS_FOUND} STREQUAL "TRUE")
   MESSAGE( STATUS  "Found PythonLibs package" )
   MESSAGE( STATUS  "Python include dirs: ${PYTHON_INCLUDE_DIRS}" )
   MESSAGE( STATUS  "Python libraries: ${PYTHON_LIBRARIES}" )
ELSE (${PYTHONLIBS_FOUND} STREQUAL "TRUE")
   MESSAGE( STATUS  "Could not locate PythonLibs package: ${PYTHONLIBS_FOUND} ")
ENDIF (${PYTHONLIBS_FOUND} STREQUAL "TRUE")
ENDIF (WITH_PYTHON STREQUAL "YES")

include( cmake/find_strus_package.cmake )

find_strus_package( core )
find_strus_package( analyzer )
find_strus_package( module )
find_strus_package( rpc )

set( CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" )
set( CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON )
configure_file( "${PROJECT_SOURCE_DIR}/src/version.hpp.in"   "${PROJECT_SOURCE_DIR}/src/version.hpp"   @ONLY )
configure_file( "${PROJECT_SOURCE_DIR}/lang/php/php.ini.in"  "${PROJECT_SOURCE_DIR}/lang/php/php.ini"  @ONLY )

set( STRUS_GETTEXT_PACKAGE "strus-dom" )	#... 1st parameter of bindtextdomain(...)
set( STRUS_GETTEXT_LOCALEDIR "" )			#... 2nd parameter of bindtextdomain(...)
configure_file( "${PROJECT_SOURCE_DIR}/src/internationalization.cpp.in"  "${PROJECT_SOURCE_DIR}/src/internationalization.cpp"  @ONLY )
configure_file( "${PROJECT_SOURCE_DIR}/dist/obs/deploy_to_obs.sh.in"  "${PROJECT_SOURCE_DIR}/dist/obs/deploy_to_obs.sh"  @ONLY )
configure_file( "${PROJECT_SOURCE_DIR}/dist/archlinux/PKGBUILD.in"  "${PROJECT_SOURCE_DIR}/dist/archlinux/PKGBUILD"  @ONLY )
configure_file( "${PROJECT_SOURCE_DIR}/dist/obs/strusbindings.dsc.in"  "${PROJECT_SOURCE_DIR}/dist/obs/strusbindings.dsc"  @ONLY )
configure_file( "${PROJECT_SOURCE_DIR}/dist/obs/strusbindings-Debian_7.0.dsc.in"  "${PROJECT_SOURCE_DIR}/dist/obs/strusbindings-Debian_7.0.dsc"  @ONLY )
configure_file( "${PROJECT_SOURCE_DIR}/dist/obs/strusbindings-xUbuntu_12.04.dsc.in"  "${PROJECT_SOURCE_DIR}/dist/obs/strusbindings-xUbuntu_12.04.dsc"  @ONLY )
configure_file( "${PROJECT_SOURCE_DIR}/dist/redhat/strusbindings.spec.in"  "${PROJECT_SOURCE_DIR}/dist/redhat/strusbindings.spec"  @ONLY )

enable_testing()

# Subdirectories:
add_subdirectory( src )
add_subdirectory( lang )
add_subdirectory( tests )
  
include( cmake/report_build_settings.cmake )

include( cmake/standard_targets.cmake )


