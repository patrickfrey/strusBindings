cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-strict-aliasing" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing" )

execute_process( COMMAND  php-config --includes  OUTPUT_VARIABLE  PHP_INCLUDES )
string( REPLACE "-I"  ""  PHP_INCLUDES  ${PHP_INCLUDES}  )
string( STRIP  ${PHP_INCLUDES}  PHP_INCLUDES )
string( REPLACE " "  ";"  PHP_INCLUDES  ${PHP_INCLUDES}  )
MESSAGE( "PHP includes: ${PHP_INCLUDES}" )
execute_process( COMMAND  php-config --extension-dir OUTPUT_VARIABLE  PHP_EXTENSION_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
MESSAGE( STATUS  "PHP module path: ${PHP_EXTENSION_DIR}" )
include_directories( "${PROJECT_SOURCE_DIR}/include"  "${PROJECT_SOURCE_DIR}/lang/php"  ${PHP_INCLUDES} )
link_directories( "${PROJECT_SOURCE_DIR}/src" "${strusmodule_LIBRARY_DIRS}" "${strusrpc_LIBRARY_DIRS}" )

# Php:
add_custom_command(
	OUTPUT strus_wrap.cpp ../strus.php php_strus.h
	PRE_BUILD
	COMMAND  swig -Werror -php -c++ -outdir ${PROJECT_SOURCE_DIR}/lang/php strus.i
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/lang/php"
	COMMENT "build PHP5 bindings" VERBATIM
	DEPENDS  strus.i strus_variant.i strus_tokenizer.i strus_normalizer.i strus_aggregator.i std_vector_string.i std_vector_int.i std_vector_rank.i std_vector_term.i exception_handler.i  strus_weighting.i strus_summarizer.i ignore_declarations.i 
)

add_library( strus_php MODULE strus_wrap.cpp objInitializers.cpp )
set_target_properties( strus_php PROPERTIES PREFIX "")
target_link_libraries( strus_php  strus_bindings )

# ------------------------------
# INSTALLATION
# ------------------------------
install(TARGETS strus_php
	LIBRARY DESTINATION ${PHP_EXTENSION_DIR} )
