cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

execute_process( COMMAND  php-config --includes  OUTPUT_VARIABLE  PHP_INCLUDES )
string( REPLACE "-I"  ""  PHP_INCLUDES  ${PHP_INCLUDES}  )
string( STRIP  ${PHP_INCLUDES}  PHP_INCLUDES )
string( REPLACE " "  ";"  PHP_INCLUDES  ${PHP_INCLUDES}  )
MESSAGE( "PHP includes: ${PHP_INCLUDES}" )
execute_process( COMMAND  php-config --extension-dir OUTPUT_VARIABLE  PHP_EXTENSION_DIR )
string( STRIP  ${PHP_EXTENSION_DIR}  PHP_EXTENSION_DIR )

include_directories( "${PROJECT_SOURCE_DIR}/include"  "${PROJECT_SOURCE_DIR}/swig/php"  ${PHP_INCLUDES} )
link_directories( "${PROJECT_SOURCE_DIR}/src" "${strusmodule_LIBRARY_DIRS}" )

# Php:
add_custom_command(
	OUTPUT strus_wrap.cpp
	PRE_BUILD
	COMMAND  swig -Werror -php -c++ -outdir ${PROJECT_SOURCE_DIR}/swig ../strus.i
	COMMAND  mv ../strus_wrap.cpp ./
	COMMAND  mv ../php_strus.h ./
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/swig/php"
	COMMENT "build PHP5 bindings" VERBATIM
	DEPENDS  ../strus.i ../strus_variant.i ../strus_tokenizer.i ../strus_normalizer.i ../std_vector_string.i ../std_vector_rank.i ../std_vector_term.i ../exception_handler.i  ../ignore_declarations.i
)

add_library( strus MODULE strus_wrap.cpp )
set_target_properties( strus PROPERTIES PREFIX "")
target_link_libraries( strus  strus_bindings )

# ------------------------------
# INSTALLATION
# ------------------------------
# Copy manually the strus.so module to the path you get with php-config --extension-dir


