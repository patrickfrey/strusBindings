cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

# --------------------------------------
# SOURCES AND INCLUDES
# --------------------------------------
set( lua_source_files
	${PROJECT_SOURCE_DIR}/lua/lapi.c
	${PROJECT_SOURCE_DIR}/lua/lauxlib.c
	${PROJECT_SOURCE_DIR}/lua/lbaselib.c
	${PROJECT_SOURCE_DIR}/lua/lbitlib.c
	${PROJECT_SOURCE_DIR}/lua/lcode.c
	${PROJECT_SOURCE_DIR}/lua/lcorolib.c
	${PROJECT_SOURCE_DIR}/lua/lctype.c
	${PROJECT_SOURCE_DIR}/lua/ldblib.c
	${PROJECT_SOURCE_DIR}/lua/ldebug.c
	${PROJECT_SOURCE_DIR}/lua/ldo.c
	${PROJECT_SOURCE_DIR}/lua/ldump.c
	${PROJECT_SOURCE_DIR}/lua/lfunc.c
	${PROJECT_SOURCE_DIR}/lua/lgc.c
	${PROJECT_SOURCE_DIR}/lua/linit.c
	${PROJECT_SOURCE_DIR}/lua/liolib.c
	${PROJECT_SOURCE_DIR}/lua/llex.c
	${PROJECT_SOURCE_DIR}/lua/lmathlib.c
	${PROJECT_SOURCE_DIR}/lua/lmem.c
	${PROJECT_SOURCE_DIR}/lua/loadlib.c
	${PROJECT_SOURCE_DIR}/lua/lobject.c
	${PROJECT_SOURCE_DIR}/lua/lopcodes.c
	${PROJECT_SOURCE_DIR}/lua/loslib.c
	${PROJECT_SOURCE_DIR}/lua/lparser.c
	${PROJECT_SOURCE_DIR}/lua/lstate.c
	${PROJECT_SOURCE_DIR}/lua/lstring.c
	${PROJECT_SOURCE_DIR}/lua/lstrlib.c
	${PROJECT_SOURCE_DIR}/lua/ltable.c
	${PROJECT_SOURCE_DIR}/lua/ltablib.c
	${PROJECT_SOURCE_DIR}/lua/ltests.c
	${PROJECT_SOURCE_DIR}/lua/ltm.c
	${PROJECT_SOURCE_DIR}/lua/lua.c
	${PROJECT_SOURCE_DIR}/lua/lundump.c
	${PROJECT_SOURCE_DIR}/lua/lutf8lib.c
	${PROJECT_SOURCE_DIR}/lua/lvm.c
	${PROJECT_SOURCE_DIR}/lua/lzio.c
)

include_directories(
	"${PROJECT_SOURCE_DIR}/lua"
	"${Boost_INCLUDE_DIRS}"
	"${Intl_INCLUDE_DIRS}"
	"${PROJECT_SOURCE_DIR}/3rdParty/textwolf/include"
	"${PROJECT_SOURCE_DIR}/include"
	"${PROJECT_SOURCE_DIR}/src"
	"${strusbase_INCLUDE_DIRS}"
	"${strus_INCLUDE_DIRS}"
	"${strusanalyzer_INCLUDE_DIRS}"
	"${strusmodule_INCLUDE_DIRS}"
	"${strustrace_INCLUDE_DIRS}"
	"${strusrpc_INCLUDE_DIRS}"
)

link_directories(
	"${Boost_LIBRARY_DIRS}"
	"${strusbase_LIBRARY_DIRS}"
)


add_library( strus_bindings_lua_impl STATIC ${lua_source_files} )


