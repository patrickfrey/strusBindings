cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

# include_directories( "${PROJECT_SOURCE_DIR}/include"  "${PROJECT_SOURCE_DIR}/swig/php"  ${PHP_INCLUDES} )
# link_directories( "${PROJECT_SOURCE_DIR}/src" "${strusmodule_LIBRARY_DIRS}" )

# Php:
add_custom_command(
	OUTPUT strus_wrap.cpp
	PRE_BUILD
	COMMAND  swig -javascript -c++ -outdir ${PROJECT_SOURCE_DIR}/swig ../strus.i
	COMMAND  mv ../strus_wrap.cpp ./
	COMMAND  mv ../php_strus.h ./
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/swig/javascript"
	COMMENT "build Javascript bindings" VERBATIM
	DEPENDS  ../strus.i )

add_library( strus_javascript SHARED strus_wrap.cpp )
target_link_libraries( strus_javascript strus_bindings )

set_target_properties(
    strus_javascript
    PROPERTIES
    DEBUG_POSTFIX "_d"
    SOVERSION "${STRUS_MAJOR_VERSION}.${STRUS_MINOR_VERSION}"
    VERSION "${STRUS_VERSION}"
)

# ------------------------------
# INSTALLATION
# ------------------------------
install( TARGETS strus_javascript
           LIBRARY DESTINATION ${LIB_INSTALL_DIR}/strus )

