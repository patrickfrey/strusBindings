cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

# must be done BEFORE project in order to work!
include(cmake/install_prefix.cmake)

# Project globals:
project( StrusBindings )
set( STRUS_MAJOR_VERSION 0 )
set( STRUS_MINOR_VERSION 17 )
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
set( CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" )
include( cmake/cxx11.cmake )
check_for_cxx11_compiler( HAVE_CXX11 )
include( cmake/dist.cmake )
include( cmake/LinuxDistribution.cmake )
include( cmake/build_rules.cmake )
include( cmake/boost.cmake)
include( cmake/link_rules.cmake )
include( cmake/standard_targets.cmake )
IF (WITH_STRUS_PATTERN STREQUAL "YES")
include( cmake/find_tre.cmake )
include( cmake/find_hyperscan.cmake )
ENDIF (WITH_STRUS_PATTERN STREQUAL "YES")
include( cmake/intl.cmake )
include( cmake/libdl.cmake )
include( cmake/cppcheck.cmake )
endif(NOT STRUS_ALL)
if (WITH_PHP STREQUAL "YES")
include( 3rdParty/papuga/cmake/php7.cmake )
endif (WITH_PHP STREQUAL "YES")
if (WITH_PYTHON STREQUAL "YES")
include( 3rdParty/papuga/cmake/python3.cmake )
endif (WITH_PYTHON STREQUAL "YES")

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
configure_file( "${PROJECT_SOURCE_DIR}/src/utils/internationalization.cpp.in"  "${CMAKE_CURRENT_BINARY_DIR}/src/utils/internationalization.cpp"  @ONLY )
configure_file( "${PROJECT_SOURCE_DIR}/dist/obs/deploy_to_obs.sh.in"  "${PROJECT_SOURCE_DIR}/dist/obs/deploy_to_obs.sh"  @ONLY )
configure_file( "${PROJECT_SOURCE_DIR}/dist/archlinux/PKGBUILD.in"  "${PROJECT_SOURCE_DIR}/dist/archlinux/PKGBUILD"  @ONLY )
configure_file( "${PROJECT_SOURCE_DIR}/dist/obs/strusbindings.dsc.in"  "${PROJECT_SOURCE_DIR}/dist/obs/strusbindings.dsc"  @ONLY )
configure_file( "${PROJECT_SOURCE_DIR}/dist/obs/strusbindings-xUbuntu_14.04.dsc.in"  "${PROJECT_SOURCE_DIR}/dist/obs/strusbindings-xUbuntu_14.04.dsc"  @ONLY )
configure_file( "${PROJECT_SOURCE_DIR}/dist/obs/strusbindings-Debian_8.0.dsc.in"  "${PROJECT_SOURCE_DIR}/dist/obs/strusbindings-Debian_8.0.dsc"  @ONLY )
configure_file( "${PROJECT_SOURCE_DIR}/dist/redhat/strusbindings.spec.in"  "${PROJECT_SOURCE_DIR}/dist/redhat/strusbindings.spec"  @ONLY )

enable_testing()

# Path declarations:
set( LUA_INCLUDE_DIRS                   "${PROJECT_SOURCE_DIR}/3rdParty/papuga/3rdParty/lua" )
set( LUA_LIBRARY_DIRS                   "${CMAKE_CURRENT_BINARY_DIR}/3rdParty/papuga/3rdParty/lua_cmake" )
set( TEXTWOLF_INCLUDE_DIRS       "${CMAKE_CURRENT_BINARY_DIR}/3rdParty/textwolf/include"  "${PROJECT_SOURCE_DIR}/3rdParty/textwolf/include" )
set( PAPUGA_INCLUDE_DIRS           "${CMAKE_CURRENT_BINARY_DIR}/3rdParty/papuga/include" "${PROJECT_SOURCE_DIR}/3rdParty/papuga/include" )
set( PAPUGA_LIBRARY_DIRS            "${CMAKE_CURRENT_BINARY_DIR}/3rdParty/papuga/src" )
set( PAPUGA_PHP7_LIBRARY_DIRS "${CMAKE_CURRENT_BINARY_DIR}/3rdParty/papuga/php7" )
set( PAPUGA_PYTHON3_LIBRARY_DIRS "${CMAKE_CURRENT_BINARY_DIR}/3rdParty/papuga/python3" )
set( PAPUGA_LUA_LIBRARY_DIRS   "${CMAKE_CURRENT_BINARY_DIR}/3rdParty/papuga/lua" )
set( BINDINGS_INCLUDE_DIRS        "${CMAKE_CURRENT_BINARY_DIR}/include"  "${PROJECT_SOURCE_DIR}/include" )
set( BINDINGS_SOURCE_DIRS         "${PROJECT_SOURCE_DIR}/src/bindings" )
set( BINDINGS_LIBRARY_DIRS         "${CMAKE_CURRENT_BINARY_DIR}/src/bindings" )
set( BINDINGS_GEN_INCLUDE_DIR  "${CMAKE_CURRENT_BINARY_DIR}/include/strus" )
set( BINDINGS_GEN_SOURCE_DIR  "${CMAKE_CURRENT_BINARY_DIR}/src/bindings" )
set( UTILS_GENSOURCE_DIR           "${CMAKE_CURRENT_BINARY_DIR}/src/utils" )
set( REQUEST_SOURCE_DIRS         "${PROJECT_SOURCE_DIR}/src/webrequest"  "${CMAKE_CURRENT_BINARY_DIR}/src/webrequest" )
set( BINDINGS_LUA_BUILD_DIR       "${CMAKE_CURRENT_BINARY_DIR}/src/lang/lua" )
set( BINDINGS_PHP7_BUILD_DIR     "${CMAKE_CURRENT_BINARY_DIR}/src/lang/php7" )
set( BINDINGS_PYTHON3_BUILD_DIR  "${CMAKE_CURRENT_BINARY_DIR}/src/lang/python3" )
set( MAIN_BINARY_DIR                     "${CMAKE_CURRENT_BINARY_DIR}" )

# Module path:
if (NOT STRUS_MODULE_PATH)
set( STRUS_MODULE_PATH  "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/strus/modules" )
endif (NOT STRUS_MODULE_PATH)
MESSAGE( STATUS  "Strus module path: ${STRUS_MODULE_PATH}" )

if(NOT STRUS_ALL)
if( DEFINED HYPERSCAN_PKGCONFIG )
	file( COPY  "${HYPERSCAN_PKGCONFIG}"  DESTINATION  ${CMAKE_BINARY_DIR}/ )
	# ... PF:HACK For hyperscan as submodule with out of source build
endif( DEFINED HYPERSCAN_PKGCONFIG )
include( cmake/report_build_settings.cmake )
endif()

# Subdirectories:
add_subdirectory( 3rdParty )
add_subdirectory( src )
add_subdirectory( tests )
  
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


