cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

# --------------------------------------
# PHP
# --------------------------------------
find_program( PHP_CONFIG_EXECUTABLE NAMES  "php-config-7*" php-config )
MESSAGE( "PHP php-config executable:  ${PHP_CONFIG_EXECUTABLE}" )

find_program( PHP_EXECUTABLE NAMES  "php-7*"  php )
MESSAGE( "PHP php executable: ${PHP_EXECUTABLE}" )

execute_process( COMMAND  ${PHP_CONFIG_EXECUTABLE} --includes  OUTPUT_VARIABLE  PHP_INCLUDES )
string( REPLACE "-I"  ""  PHP_INCLUDE_DIRS  ${PHP_INCLUDES}  )
string( STRIP  ${PHP_INCLUDE_DIRS}  PHP_INCLUDE_DIRS )
string( REPLACE " "  ";"  PHP_INCLUDE_DIRS  ${PHP_INCLUDE_DIRS}  )
MESSAGE( "PHP include dirs: ${PHP_INCLUDE_DIRS}" )

execute_process( COMMAND  ${PHP_CONFIG_EXECUTABLE} --ldflags  OUTPUT_VARIABLE PHP_LDFLAGS )
string( REPLACE "-L"  ""  PHP_LIBRARY_DIRS  ${PHP_LDFLAGS}  )
string( STRIP  ${PHP_LIBRARY_DIRS}  PHP_LIBRARY_DIRS )
MESSAGE( "PHP library dirs: ${PHP_LIBRARY_DIRS}" )

execute_process( COMMAND  ${PHP_CONFIG_EXECUTABLE} --libs OUTPUT_VARIABLE PHP_LIBS )
string( REPLACE "-l"  ""  PHP_LIBRARIES  ${PHP_LIBS}  )
string( STRIP  ${PHP_LIBRARIES}  PHP_LIBRARIES )
set( PHP_LIBRARIES ${PHP_LIBRARIES} )
MESSAGE( "PHP libraries: ${PHP_LIBRARIES}" )

execute_process( COMMAND  ${PHP_CONFIG_EXECUTABLE} --extension-dir  OUTPUT_VARIABLE  PHP_EXTENSION_DIR )
string( STRIP  "${PHP_EXTENSION_DIR}"  PHP_EXTENSION_DIR )
MESSAGE( "PHP extension dir: ${PHP_EXTENSION_DIR}" )

execute_process( COMMAND  ${PHP_EXECUTABLE}  ${PROJECT_SOURCE_DIR}/src/lang/php7/getPhpIniFile.php  OUTPUT_VARIABLE  PHP_INI_FILE )
string( STRIP  "${PHP_INI_FILE}"  PHP_INI_FILE )
MESSAGE( "PHP .ini file: ${PHP_INI_FILE}" )

