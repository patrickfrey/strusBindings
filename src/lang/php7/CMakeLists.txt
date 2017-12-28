cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

# --------------------------------------
# PHP
# --------------------------------------
include_directories(
        ${PHP7_INCLUDE_DIRS}
	${Intl_INCLUDE_DIRS}
	${PAPUGA_INCLUDE_DIRS}
	${BINDINGS_INCLUDE_DIRS}
)

link_directories(
        ${PHP7_LIBRARY_DIRS}
        ${PAPUGA_PHP7_LIBRARY_DIRS}
	${PAPUGA_LIBRARY_DIRS}
	${BINDINGS_LIBRARY_DIRS}
	${strusbase_LIBRARY_DIRS}
	${strusmodule_LIBRARY_DIRS}
	${strusrpc_LIBRARY_DIRS}
	${strus_LIBRARY_DIRS}
	${strusanalyzer_LIBRARY_DIRS}
	${strustrace_LIBRARY_DIRS}
)

add_custom_command(
    OUTPUT      strus.c
    DEPENDS    ${CMAKE_CURRENT_BINARY_DIR}/../../../include/strus/bindingObjects.h strusBindingsModuleGen strus_bindings_description
    COMMAND  strusBindingsModuleGen php7 module strus.c 
    COMMENT  "Generating PHP 7 module source strus.c"
    VERBATIM
 )
 
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/strusdoc.php
    DEPENDS   ${CMAKE_CURRENT_BINARY_DIR}/../../../include/strus/bindingObjects.h strus_bindings_description strusBindingsModuleGen
    COMMAND  strusBindingsModuleGen php7 doc ${CMAKE_CURRENT_BINARY_DIR}/strusdoc.php
    COMMENT  "Generating bindings interface doc description file ${CMAKE_CURRENT_BINARY_DIR}/strusdoc.php"
    VERBATIM
 )

add_custom_command(
    OUTPUT      php.ini
    DEPENDS    strusBindingsModuleGen strus_bindings_description
    COMMAND  strusBindingsModuleGen php7 ini php.ini "phpini=${PHP7_INI_FILE}"
    COMMENT  "Generating PHP 7 .ini file with strus extension loaded"
    VERBATIM
 )

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/doc/phpBindingsDoc.htm
    DEPENDS   papugaDoc  "${PROJECT_SOURCE_DIR}/src/lang/php7/strusPhpDoc.tpl"  ${CMAKE_CURRENT_BINARY_DIR}/strusdoc.php
    COMMAND  papugaDoc -o ${CMAKE_BINARY_DIR}/doc/phpBindingsDoc.htm "${PROJECT_SOURCE_DIR}/src/lang/php7/strusPhpDoc.tpl" ${CMAKE_CURRENT_BINARY_DIR}/strusdoc.php
    COMMENT  "Generating bindings interface doc html file ${CMAKE_BINARY_DIR}/doc/phpBindingsDoc.htm"
    VERBATIM
 )

configure_file( ${PROJECT_SOURCE_DIR}/src/lang/php7/phpBindingsDoc.css  ${CMAKE_BINARY_DIR}/doc/phpBindingsDoc.css  COPYONLY )
configure_file( ${PROJECT_SOURCE_DIR}/src/lang/php7/phpBindingsDoc.js  ${CMAKE_BINARY_DIR}/doc/phpBindingsDoc.js  COPYONLY )

 add_custom_target(
    php7_mod ALL
    DEPENDS strus.c php.ini ${CMAKE_CURRENT_BINARY_DIR}/strusdoc.php papuga_php7_gen ${CMAKE_BINARY_DIR}/doc/phpBindingsDoc.htm ${CMAKE_BINARY_DIR}/doc/phpBindingsDoc.css ${CMAKE_BINARY_DIR}/doc/phpBindingsDoc.js )

# ------------------------------
# LIBRARIES
# ------------------------------
add_library( strus_bindings_php7 MODULE strus.c )
add_dependencies( strus_bindings_php7 php7_mod )
target_link_libraries( strus_bindings_php7 papuga_devel papuga_php7_dev strus_bindings ${Intl_LIBRARIES} )

if(APPLE)
set_target_properties( strus_bindings_php7 PROPERTIES LINK_FLAGS "-undefined dynamic_lookup" )
endif(APPLE)

set_target_properties( strus_bindings_php7 PROPERTIES OUTPUT_NAME strus PREFIX "" )

# Copy Php7 module into a created directory easy relocatable by tests:
add_custom_command( TARGET strus_bindings_php7 POST_BUILD
					  COMMAND ${CMAKE_COMMAND} -E make_directory  ${CMAKE_BINARY_DIR}/modules/php7 )
add_custom_command( TARGET strus_bindings_php7 POST_BUILD
					  COMMAND ${CMAKE_COMMAND} -E copy_if_different  $<TARGET_FILE:strus_bindings_php7>  ${CMAKE_BINARY_DIR}/modules/php7/
					  COMMENT "Copy Php7 module built to ${CMAKE_BINARY_DIR}/modules/php7/" )

# ------------------------------
# INSTALLATION
# ------------------------------
install( TARGETS strus_bindings_php7
           LIBRARY DESTINATION "${PHP7_EXTENSION_DIR}" )



