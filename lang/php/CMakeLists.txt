cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-strict-aliasing -Wno-unused-label -Wno-unused-variable" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing -Wno-unused-label -Wno-unused-variable" )

find_program( PHP_CONFIG_EXECUTABLE NAMES php-config-5.6 php-config56 php-config5 php-config )
MESSAGE( "PHP php-config executable: ${PHP_CONFIG_EXECUTABLE}" )

find_program( PHP_EXECUTABLE NAMES php-5.6 php5 php56 php )
MESSAGE( "PHP php executable: ${PHP_EXECUTABLE}" )

execute_process( COMMAND  ${PHP_CONFIG_EXECUTABLE} --includes  OUTPUT_VARIABLE  PHP_INCLUDES )
string( REPLACE "-I"  ""  PHP_INCLUDES  ${PHP_INCLUDES}  )
string( STRIP  ${PHP_INCLUDES}  PHP_INCLUDES )
string( REPLACE " "  ";"  PHP_INCLUDES  ${PHP_INCLUDES}  )
MESSAGE( "PHP includes: ${PHP_INCLUDES}" )

execute_process( COMMAND  ${PHP_CONFIG_EXECUTABLE} --ldflags  OUTPUT_VARIABLE PHP_LDFLAGS )
string( STRIP  ${PHP_LDFLAGS}  PHP_LDFLAGS )
MESSAGE( "PHP linker flags: ${PHP_LDFLAGS}" )

execute_process( COMMAND  ${PHP_CONFIG_EXECUTABLE} --libs OUTPUT_VARIABLE PHP_LIBS )
string( STRIP  ${PHP_LIBS}  PHP_LIBS )  
MESSAGE( "PHP libraries: ${PHP_LIBS}" )

execute_process( COMMAND  ${PHP_CONFIG_EXECUTABLE} --extension-dir OUTPUT_VARIABLE  PHP_EXTENSION_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
MESSAGE( STATUS  "PHP module path: ${PHP_EXTENSION_DIR}" )
include_directories( "${PROJECT_SOURCE_DIR}/include"  "${PROJECT_SOURCE_DIR}/lang/php"  ${PHP_INCLUDES} "${Boost_INCLUDE_DIRS}")
link_directories( "${PROJECT_SOURCE_DIR}/src" "${strusmodule_LIBRARY_DIRS}" "${strusrpc_LIBRARY_DIRS}" )

# Php:
add_custom_command(
	OUTPUT strus_wrap.cpp strus.php php_strus.h
	PRE_BUILD
	COMMAND  ${SWIG_EXECUTABLE} -Werror -php -c++ -outdir ${PROJECT_SOURCE_DIR}/lang/php strus.i
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/lang/php"
	COMMENT "build PHP5 bindings" VERBATIM
	DEPENDS  strus.i strus_variant.i strus_tokenizer.i strus_normalizer.i strus_aggregator.i std_vector_string.i std_vector_int.i std_vector_rank.i std_vector_term.i exception_handler.i  strus_weighting.i strus_summarizer.i ignore_declarations.i "${PROJECT_SOURCE_DIR}/include/strus/bindingObjects.hpp"
)

add_library( strus_php MODULE strus_wrap.cpp objInitializers.cpp )
set_target_properties( strus_php PROPERTIES PREFIX "" OUTPUT_NAME strus )
if(APPLE)
	set_target_properties( strus_php PROPERTIES LINK_FLAGS "${PHP_LDFLAGS} -Wl,-undefined,dynamic_lookup" )
else(APPLE)
	set_target_properties( strus_php PROPERTIES LINK_FLAGS "${PHP_LDFLAGS}" )
endif(APPLE)
if(APPLE)
	target_link_libraries( strus_php  strus_bindings ${PHP_LIBS} )
else(APPLE)
	target_link_libraries( strus_php  strus_bindings )
endif(APPLE)

add_subdirectory( example )

# ------------------------------
# INSTALLATION
# ------------------------------
install( TARGETS strus_php
	LIBRARY DESTINATION ${PHP_EXTENSION_DIR} )

