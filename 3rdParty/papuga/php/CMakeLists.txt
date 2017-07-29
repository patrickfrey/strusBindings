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


# --------------------------------------
# SOURCES AND INCLUDES
# --------------------------------------
set( php7_dev_source_files
	libpapuga_php7_dev.c
)

set( php7_gen_source_files
	printPhp7Doc.cpp
	printPhp7Mod.cpp
        libpapuga_php7_gen.cpp
)

include_directories(
	"${Intl_INCLUDE_DIRS}"
	"${PROJECT_SOURCE_DIR}/3rdParty/fmt"
	"${PROJECT_SOURCE_DIR}/3rdParty/php"
	"${PROJECT_SOURCE_DIR}/3rdParty/papuga/include"
	"${PROJECT_SOURCE_DIR}/include"
	${PHP_INCLUDE_DIRS}
)

link_directories(
	"${PROJECT_SOURCE_DIR}/3rdParty/fmt/fmt"
	${PHP_LIBRARY_DIRS}
)


# ------------------------------
# LIBRARIES
# ------------------------------
add_library( papuga_php7_gen SHARED ${php7_gen_source_files} )
target_link_libraries( papuga_php7_gen fmt papuga_gen_utils )

set_target_properties(
    papuga_php7_gen
    PROPERTIES
    DEBUG_POSTFIX "_d"
    SOVERSION "${PAPUGA_MAJOR_VERSION}.${PAPUGA_MINOR_VERSION}"
    VERSION ${PAPUGA_VERSION}
)

add_library( papuga_php7_dev SHARED ${php7_dev_source_files} )
target_link_libraries( papuga_php7_dev papuga_gen_utils papuga_devel )

set_target_properties(
    papuga_php7_dev
    PROPERTIES
    DEBUG_POSTFIX "_d"
    SOVERSION "${PAPUGA_MAJOR_VERSION}.${PAPUGA_MINOR_VERSION}"
    VERSION ${PAPUGA_VERSION}
)


# ------------------------------
# INSTALLATION
# ------------------------------
install( TARGETS papuga_php7_gen
           LIBRARY DESTINATION ${LIB_INSTALL_DIR}/papuga )

install( TARGETS papuga_php7_dev
           LIBRARY DESTINATION ${LIB_INSTALL_DIR}/papuga )



