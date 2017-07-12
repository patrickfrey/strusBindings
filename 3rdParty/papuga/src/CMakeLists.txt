cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

# --------------------------------------
# SOURCES AND INCLUDES
# --------------------------------------
set( source_files
	allocator.c
	serialization.c
	callResult.c
	errors.c
	errors.cpp
	valueVariant.cpp
	serialization.cpp
)

include_directories(
	"${Intl_INCLUDE_DIRS}"
	"${PROJECT_SOURCE_DIR}/src"
	"${PROJECT_SOURCE_DIR}/3rdParty/textwolf/include"
	"${PROJECT_SOURCE_DIR}/3rdParty/papuga/include"
)

link_directories(
)

# ------------------------------
# LIBRARIES
# ------------------------------
add_library( papuga_devel STATIC ${source_files} )

add_library( papuga_doc_gen SHARED libpapuga_doc_gen.cpp )
target_link_libraries( papuga_doc_gen )

set_target_properties(
    papuga_doc_gen
    PROPERTIES
    DEBUG_POSTFIX "_d"
    SOVERSION "${PAPUGA_MAJOR_VERSION}.${PAPUGA_MINOR_VERSION}"
    VERSION ${PAPUGA_VERSION}
)

# -------------------------------------------
# DOC GENERATOR
# -------------------------------------------
add_executable( papugaDoc papugaDoc.cpp )
target_link_libraries( papugaDoc papuga_doc_gen ${CMAKE_DL_LIBS} ${Intl_LIBRARIES} )

# ------------------------------
# INSTALLATION
# ------------------------------
install( TARGETS papugaDoc
           RUNTIME DESTINATION bin )

