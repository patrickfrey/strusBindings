cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

# --------------------------------------
# INCLUDES & LINKDIRS
# --------------------------------------
include_directories(
	"${Boost_INCLUDE_DIRS}"
	"${Intl_INCLUDE_DIRS}"
	"${CMAKE_BINARY_DIR}/include"
	"${PROJECT_SOURCE_DIR}/3rdParty/papuga/include"
	"${PROJECT_SOURCE_DIR}/include"
	"${PROJECT_SOURCE_DIR}/src/gen"
	"${strusbase_INCLUDE_DIRS}"
)

link_directories(
	"${Boost_LIBRARY_DIRS}"
	"${strusbase_LIBRARY_DIRS}"
	"${PROJECT_SOURCE_DIR}/3rdParty/papuga/src"
)

# --------------------------------------
# C INTERFACE CODE GEN
# --------------------------------------
file( MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/include/strus/lib )

add_custom_command(
    OUTPUT   
                ${CMAKE_BINARY_DIR}/include/strus/bindingObjects.h
		${CMAKE_BINARY_DIR}/include/strus/lib/bindings_description.hpp
	        ${CMAKE_BINARY_DIR}/src/bindings/bindingObjects.cpp
		${CMAKE_BINARY_DIR}/src/bindings/bindingClassIds.hpp
		${CMAKE_BINARY_DIR}/src/bindings/bindingClassTemplate.hpp
	        ${CMAKE_BINARY_DIR}/src/bindings/libstrus_bindings_description.cpp
    DEPENDS   strusBindingsInterfaceGen
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/bindings/impl/context.hpp
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/bindings/impl/storage.hpp
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/bindings/impl/vector.hpp
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/bindings/impl/analyzer.hpp
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/bindings/impl/query.hpp
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/bindings/impl/value/iterator.hpp
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/bindings/impl/value/struct.hpp
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/bindings/impl/value/objectref.hpp
    COMMAND  ${CMAKE_BINARY_DIR}/src/parser/strusBindingsInterfaceGen ${CMAKE_BINARY_DIR}  ${PROJECT_SOURCE_DIR}/src/bindings/impl/context.hpp ${PROJECT_SOURCE_DIR}/src/bindings/impl/storage.hpp ${PROJECT_SOURCE_DIR}/src/bindings/impl/vector.hpp ${PROJECT_SOURCE_DIR}/src/bindings/impl/analyzer.hpp ${PROJECT_SOURCE_DIR}/src/bindings/impl/query.hpp
    COMMENT  "Generating strus bindings C header and implementation (output root dir: ${CMAKE_BINARY_DIR})"
    VERBATIM
 )
 
 add_custom_target(
    strus_bindings_c_interface ALL
    DEPENDS ${CMAKE_BINARY_DIR}/src/bindings/bindingObjects.cpp ${CMAKE_BINARY_DIR}/include/strus/bindingObjects.h ${CMAKE_BINARY_DIR}/include/strus/lib/bindings_description.hpp ${CMAKE_BINARY_DIR}/src/bindings/bindingClassIds.hpp ${CMAKE_BINARY_DIR}/src/bindings/bindingClassTemplate.hpp ${CMAKE_BINARY_DIR}/src/bindings/libstrus_bindings_description.cpp
)

# ------------------------------
# PROGRAMS
# ------------------------------
add_executable( strusBindingsModuleGen strusBindingsModuleGen.cpp )
target_link_libraries( strusBindingsModuleGen  "${Boost_LIBRARIES}" strus_base papuga_lua_gen papuga_php7_gen strus_bindings_description ${Intl_LIBRARIES} )

