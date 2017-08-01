cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

# --------------------------------------
# PHP
# --------------------------------------
include( ./php.cmake )

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



