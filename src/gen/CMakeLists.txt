cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

# --------------------------------------
# INCLUDES & LINKDIRS
# --------------------------------------
include_directories(
	${Boost_INCLUDE_DIRS}
	"${Intl_INCLUDE_DIRS}"
	"${CMAKE_CURRENT_BINARY_DIR}/../../3rdParty/papuga/include"
	"${PROJECT_SOURCE_DIR}/3rdParty/papuga/include"
	"${CMAKE_CURRENT_BINARY_DIR}/../../include"
	"${PROJECT_SOURCE_DIR}/include"
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}/src/gen"
	"${strusbase_INCLUDE_DIRS}"
)

link_directories(
	${Boost_LIBRARY_DIRS}
	"${strusbase_LIBRARY_DIRS}"
	"${CMAKE_CURRENT_BINARY_DIR}/../../3rdParty/papuga/src"
)

# --------------------------------------
# C INTERFACE CODE GEN
# --------------------------------------
file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../../include/strus/lib )

add_custom_command(
    OUTPUT   
                ${CMAKE_CURRENT_BINARY_DIR}/../../include/strus/bindingObjects.h
		${CMAKE_CURRENT_BINARY_DIR}/../../include/strus/lib/bindings_description.hpp
	        ${CMAKE_CURRENT_BINARY_DIR}/../bindings/bindingObjects.cpp
		${CMAKE_CURRENT_BINARY_DIR}/../bindings/bindingClassTemplate.hpp
	        ${CMAKE_CURRENT_BINARY_DIR}/../bindings/libstrus_bindings_description.cpp
    DEPENDS   strusBindingsInterfaceGen
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/bindings/impl/structs.hpp
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/bindings/impl/context.hpp
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/bindings/impl/storage.hpp
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/bindings/impl/vector.hpp
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/bindings/impl/analyzer.hpp
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/bindings/impl/query.hpp
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/bindings/impl/value/forwardTermsIterator.hpp
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/bindings/impl/value/selectIterator.hpp
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/bindings/impl/value/iterator.hpp
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/bindings/impl/value/statisticsIterator.hpp
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/bindings/impl/value/metadataExpression.hpp
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/bindings/impl/value/struct.hpp
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/bindings/impl/value/objectref.hpp
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/bindings/impl/value/termExpression.hpp
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/bindings/impl/value/postingIterator.hpp
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/bindings/impl/value/valueIterator.hpp
    DEPENDS   ${PROJECT_SOURCE_DIR}/src/bindings/impl/value/searchTermsIterator.hpp
    COMMAND  strusBindingsInterfaceGen ${CMAKE_CURRENT_BINARY_DIR}/../../  ${PROJECT_SOURCE_DIR}/src/bindings/impl/structs.hpp ${PROJECT_SOURCE_DIR}/src/bindings/impl/context.hpp ${PROJECT_SOURCE_DIR}/src/bindings/impl/storage.hpp ${PROJECT_SOURCE_DIR}/src/bindings/impl/vector.hpp ${PROJECT_SOURCE_DIR}/src/bindings/impl/analyzer.hpp ${PROJECT_SOURCE_DIR}/src/bindings/impl/query.hpp
    COMMENT  "Generating strus bindings C header and implementation (output root dir: ${CMAKE_CURRENT_BINARY_DIR})"
    VERBATIM
 )
 
 add_custom_target(
    strus_bindings_c_interface ALL
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/../bindings/bindingObjects.cpp ${CMAKE_CURRENT_BINARY_DIR}/../../include/strus/bindingObjects.h ${CMAKE_CURRENT_BINARY_DIR}/../../include/strus/lib/bindings_description.hpp ${CMAKE_CURRENT_BINARY_DIR}/../bindings/bindingClassTemplate.hpp ${CMAKE_CURRENT_BINARY_DIR}/../bindings/libstrus_bindings_description.cpp
)

# ------------------------------
# PROGRAMS
# ------------------------------
IF (WITH_PHP STREQUAL "YES")
set( PHP7_GEN_LIBS  papuga_php7_gen )
ENDIF (WITH_PHP STREQUAL "YES")
IF (WITH_PYTHON STREQUAL "YES")
set( PYTHON3_GEN_LIBS  papuga_python3_gen )
ENDIF (WITH_PYTHON STREQUAL "YES")
add_executable( strusBindingsModuleGen strusBindingsModuleGen.cpp )
target_link_libraries( strusBindingsModuleGen  "${Boost_LIBRARIES}" strus_base papuga_lua_gen ${PHP7_GEN_LIBS} ${PYTHON3_GEN_LIBS} strus_bindings_description ${Intl_LIBRARIES} )

