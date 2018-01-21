cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# --------------------------------------
# SOURCES AND INCLUDES
# --------------------------------------
set( source_files
	${UTILS_GENSOURCE_DIR}/internationalization.cpp
)

include_directories(
	${Boost_INCLUDE_DIRS}
	"${Intl_INCLUDE_DIRS}"
	"${BINDINGS_INCLUDE_DIRS}"
	"${strusbase_INCLUDE_DIRS}"
)
link_directories(
	${Boost_LIBRARY_DIRS}
	"${MAIN_SOURCE_DIR}/utils"
	"${strusbase_LIBRARY_DIRS}"
)


# -------------------------------------------
# UTILS LIBRARY
# Library with some helper functions used by strus libraries and programs but not part of the public interface
# -------------------------------------------
add_library( strusbindings_private_utils STATIC ${source_files} )
target_link_libraries( strusbindings_private_utils ${Intl_LIBRARIES} ${Boost_LIBRARIES} )
set_property( TARGET strusbindings_private_utils PROPERTY POSITION_INDEPENDENT_CODE TRUE )

# ------------------------------
# INSTALLATION
# ------------------------------
# none

