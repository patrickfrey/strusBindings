cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

# --------------------------------------
# SOURCES AND INCLUDES
# --------------------------------------
set( source_files
	stringBuffer.c
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


