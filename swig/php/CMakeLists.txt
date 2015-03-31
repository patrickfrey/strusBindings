cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

execute_process( COMMAND  php-config5 --includes  OUTPUT_VARIABLE  PHP_INCLUDES )
string( REPLACE "-I"  ""  PHP_INCLUDES  ${PHP_INCLUDES}  )
string( STRIP  ${PHP_INCLUDES}  PHP_INCLUDES )
string( REPLACE " "  ";"  PHP_INCLUDES  ${PHP_INCLUDES}  )
MESSAGE( "PHP includes: ${PHP_INCLUDES}" )

include_directories( "${PROJECT_SOURCE_DIR}/include"  ${PHP_INCLUDES} )
link_directories( "${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/swig/php" )

# Php:
add_custom_command(
	OUTPUT strus_wrap.cpp
	PRE_BUILD
	COMMAND  swig -php -c++ -outdir ${PROJECT_SOURCE_DIR}/swig ${PROJECT_SOURCE_DIR}/swig/strus.i
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/swig/php"
	COMMENT "build PHP5 bindings" VERBATIM
	DEPENDS  ${PROJECT_SOURCE_DIR}/swig/strus.i )

add_library( strus_php SHARED ${PROJECT_SOURCE_DIR}/swig/strus_wrap.cpp )
target_link_libraries( strus_php strus_bindings )

set_target_properties(
    strus_php
    PROPERTIES
    DEBUG_POSTFIX "_d"
    SOVERSION "${STRUS_MAJOR_VERSION}.${STRUS_MINOR_VERSION}"
    VERSION ${STRUS_VERSION}
)

# ------------------------------
# INSTALLATION
# ------------------------------
install( TARGETS strus_php
           LIBRARY DESTINATION lib/strus )

