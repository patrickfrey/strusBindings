cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

# --------------------------------------
# SOURCES AND INCLUDES
# --------------------------------------
set( dev_source_files
	libpapuga_lua_dev.c
)

set( gen_source_files
        libpapuga_lua_gen.cpp
)

include_directories(
	"${Intl_INCLUDE_DIRS}"
	"${PROJECT_SOURCE_DIR}/3rdParty/fmt"
	"${PROJECT_SOURCE_DIR}/3rdParty/lua"
	"${PROJECT_SOURCE_DIR}/3rdParty/papuga/include"
	"${PROJECT_SOURCE_DIR}/include"
)

link_directories(
	"${PROJECT_SOURCE_DIR}/3rdParty/fmt/fmt"
)


# ------------------------------
# LIBRARIES
# ------------------------------
add_library( papuga_lua_gen SHARED ${gen_source_files} )
target_link_libraries( papuga_lua_gen fmt papuga_gen_utils )

set_target_properties(
    papuga_lua_gen
    PROPERTIES
    DEBUG_POSTFIX "_d"
    SOVERSION "${PAPUGA_MAJOR_VERSION}.${PAPUGA_MINOR_VERSION}"
    VERSION ${PAPUGA_VERSION}
)

add_library( papuga_lua_dev SHARED ${dev_source_files} )
target_link_libraries( papuga_lua_dev )

set_target_properties(
    papuga_lua_dev
    PROPERTIES
    DEBUG_POSTFIX "_d"
    SOVERSION "${PAPUGA_MAJOR_VERSION}.${PAPUGA_MINOR_VERSION}"
    VERSION ${PAPUGA_VERSION}
)


# ------------------------------
# INSTALLATION
# ------------------------------
install( TARGETS papuga_lua_gen
           LIBRARY DESTINATION ${LIB_INSTALL_DIR}/papuga )

install( TARGETS papuga_lua_dev
           LIBRARY DESTINATION ${LIB_INSTALL_DIR}/papuga )



