cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

# --------------------------------------
# SOURCES AND INCLUDES
# --------------------------------------
set( lua_source_files
	papugaLuaHelpers.cpp
)

set( source_files
        libpapuga_lua.cpp
)

include_directories(
	"${Intl_INCLUDE_DIRS}"
	/home/patrick/Projects/github/strusBindings/3rdParty/lua
	"${PROJECT_SOURCE_DIR}/include"
)

# ------------------------------
# LIBRARIES
# ------------------------------
add_library( papuga_lua SHARED ${source_files} ${lua_source_files} )
target_link_libraries( papuga_lua )

set_target_properties(
    papuga_lua
    PROPERTIES
    DEBUG_POSTFIX "_d"
    SOVERSION "${PAPUGA_MAJOR_VERSION}.${PAPUGA_MINOR_VERSION}"
    VERSION ${PAPUGA_VERSION}
)

# ------------------------------
# INSTALLATION
# ------------------------------
install( TARGETS papuga_lua
           LIBRARY DESTINATION ${LIB_INSTALL_DIR}/papuga )


