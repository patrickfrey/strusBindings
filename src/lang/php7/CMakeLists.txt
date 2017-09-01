cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

# --------------------------------------
# PHP
# --------------------------------------
include( ${PROJECT_SOURCE_DIR}/3rdParty/papuga/php7/php.cmake )

include_directories(
        "${PHP7_INCLUDE_DIRS}"
	"${Intl_INCLUDE_DIRS}"
	"${CMAKE_BINARY_DIR}/include"
	"${PROJECT_SOURCE_DIR}/3rdParty/papuga/include"
	"${PROJECT_SOURCE_DIR}/include"
)

link_directories(
        "${PHP7_LIBRARY_DIRS}"
	"${PROJECT_SOURCE_DIR}/3rdParty/papuga/src"
	"${PROJECT_SOURCE_DIR}/src/bindings"
	"${PROJECT_SOURCE_DIR}/src/lang/php7"
	"${strusbase_LIBRARY_DIRS}"
	"${strusmodule_LIBRARY_DIRS}"
	"${strusrpc_LIBRARY_DIRS}"
	"${strus_LIBRARY_DIRS}"
	"${strusanalyzer_LIBRARY_DIRS}"
	"${strustrace_LIBRARY_DIRS}"
)

add_custom_command(
    OUTPUT      strus.c
    DEPENDS    ${CMAKE_BINARY_DIR}/include/strus/bindingObjects.h strusBindingsModuleGen strus_bindings_description
    COMMAND  ${CMAKE_BINARY_DIR}/src/gen/strusBindingsModuleGen php7 module strus.c 
    COMMENT  "Generating PHP 7 module source strus.c"
    VERBATIM
 )
 
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/src/lang/php7/strus.php
    DEPENDS   ${CMAKE_BINARY_DIR}/include/strus/bindingObjects.h strus_bindings_description strusBindingsModuleGen
    COMMAND  ${CMAKE_BINARY_DIR}/src/gen/strusBindingsModuleGen php7 doc ${CMAKE_BINARY_DIR}/src/lang/php7/strus.php
    COMMENT  "Generating bindings interface doc description file ${CMAKE_BINARY_DIR}/src/lang/php7/strus.php"
    VERBATIM
 )

add_custom_command(
    OUTPUT      php.ini
    DEPENDS    strusBindingsModuleGen strus_bindings_description
    COMMAND  ${CMAKE_BINARY_DIR}/src/gen/strusBindingsModuleGen php7 ini php.ini "phpini=${PHP7_INI_FILE}"
    COMMENT  "Generating PHP 7 .ini file with strus extension loaded"
    VERBATIM
 )
 
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/doc/phpBindingsDoc.htm
    DEPENDS   papugaDoc  "${PROJECT_SOURCE_DIR}/src/lang/php7/strusPhpDoc.tpl"  ${CMAKE_BINARY_DIR}/src/lang/php7/strus.php
    COMMAND  papugaDoc -o ${CMAKE_BINARY_DIR}/doc/phpBindingsDoc.htm "${PROJECT_SOURCE_DIR}/src/lang/php7/strusPhpDoc.tpl" ${CMAKE_BINARY_DIR}/src/lang/php7/strus.php
    COMMENT  "Generating bindings interface doc html file ${CMAKE_BINARY_DIR}/src/lang/php7/strus.php"
    VERBATIM
 )

 add_custom_target(
    php7_mod ALL
    DEPENDS strus.c php.ini ${CMAKE_BINARY_DIR}/src/lang/php7/strus.php papuga_php7_gen ${CMAKE_BINARY_DIR}/doc/phpBindingsDoc.htm
)

# ------------------------------
# LIBRARIES
# ------------------------------
add_library( strus_bindings_php7 MODULE strus.c )
add_dependencies( strus_bindings_php7 php7_mod )
target_link_libraries( strus_bindings_php7 papuga_devel papuga_php7_dev strus_bindings )

set_target_properties( strus_bindings_php7 PROPERTIES OUTPUT_NAME strus PREFIX "" )

# ------------------------------
# INSTALLATION
# ------------------------------
install( TARGETS strus_bindings_php7
           LIBRARY DESTINATION "${PHP7_EXTENSION_DIR}" )



