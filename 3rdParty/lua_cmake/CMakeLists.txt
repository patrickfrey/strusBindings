cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

if ( WIN32 )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLUA_USE_WINDOWS" )
endif( WIN32 )
if( APPLE )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLUA_USE_MACOSX" )
endif( APPLE )
if( UNIX )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLUA_USE_LINUX" )
endif( UNIX )

# --------------------------------------
# SOURCES AND INCLUDES
# --------------------------------------
set( source_files
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lapi.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lauxlib.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lbaselib.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lbitlib.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lcode.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lcorolib.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lctype.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/ldblib.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/ldebug.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/ldo.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/ldump.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lfunc.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lgc.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/linit.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/liolib.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/llex.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lmathlib.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lmem.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/loadlib.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lobject.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lopcodes.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/loslib.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lparser.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lstate.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lstring.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lstrlib.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/ltable.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/ltablib.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/ltests.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/ltm.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lua.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lundump.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lutf8lib.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lvm.c
	${PROJECT_SOURCE_DIR}/3rdParty/lua/lzio.c
)

include_directories(
	"${Intl_INCLUDE_DIRS}"
	"${PROJECT_SOURCE_DIR}/3rdParty/lua"
)

link_directories(
)

# ------------------------------
# LIBRARIES
# ------------------------------
add_library( strus_lua STATIC ${source_files} )

