cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

include_directories(
        ${LUA_INCLUDE_DIRS}
	${Intl_INCLUDE_DIRS}
	${PAPUGA_INCLUDE_DIRS}
	${BINDINGS_INCLUDE_DIRS}
)

link_directories(
	${Boost_LIBRARY_DIRS}
        ${LUA_LIBRARY_DIRS}
        ${PAPUGA_LUA_LIBRARY_DIRS}
	${PAPUGA_LIBRARY_DIRS}
	${BINDINGS_LIBRARY_DIRS}
	${strusbase_LIBRARY_DIRS}
	${strusmodule_LIBRARY_DIRS}
	${strusrpc_LIBRARY_DIRS}
	${strus_LIBRARY_DIRS}
	${strusanalyzer_LIBRARY_DIRS}
	${strustrace_LIBRARY_DIRS}
)

# ------------------------------
# PROGRAMS
# ------------------------------
add_executable( strusWebRequestLuaSim strusWebRequestLuaSim.cpp )
target_link_libraries( strusWebRequestLuaSim strus_webrequest strus_base strus_error strus_bindings_lua papuga_devel papuga_lua m ${Intl_LIBRARIES} )

# FreeBSD needs kernel data access library for libuv (-libkvm)
find_library( LIBKVM_LIBRARIES kvm )
if(LIBKVM_LIBRARIES)
        target_link_libraries( strusWebRequestLuaSim ${LIBKVM_LIBRARIES} )
endif()

if( LIBDL_LIBRARY )
        target_link_libraries( strusWebRequestLuaSim ${LIBDL_LIBRARY} )
endif()

# ------------------------------
# INSTALLATION
# ------------------------------
install( TARGETS strusWebRequestLuaSim
           RUNTIME DESTINATION bin )



