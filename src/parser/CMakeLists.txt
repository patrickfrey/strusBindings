cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# --------------------------------------
# SOURCES AND INCLUDES
# --------------------------------------
set( source_files
	printFrame.cpp
	fillTypeTables.cpp
	interfaceParser.cpp
)

include_directories(
	"${Boost_INCLUDE_DIRS}"
	"${Intl_INCLUDE_DIRS}"
	"${PROJECT_SOURCE_DIR}/include"
	"${PROJECT_SOURCE_DIR}/3rdParty/papuga/include"
	"${strusbase_INCLUDE_DIRS}"
	"${strus_INCLUDE_DIRS}"
	"${strusanalyzer_INCLUDE_DIRS}" )
	"${Boost_LIBRARY_DIRS}"
	"${strusbase_LIBRARY_DIRS}"
	"${strus_LIBRARY_DIRS}"
	"${strusanalyzer_LIBRARY_DIRS}" )


# -------------------------------------------
# STRUS BINDINGS CODE GENERATOR
# Code generator that maps the strus interfaces to sources needed for bindings
# -------------------------------------------
add_executable( strusBindingsInterfaceGen strusBindingsInterfaceGen.cpp ${source_files} )
target_link_libraries( strusBindingsInterfaceGen strus_base ${CMAKE_DL_LIBS} ${Intl_LIBRARIES} )

# ------------------------------
# INSTALLATION
# ------------------------------
install( TARGETS strusBindingsInterfaceGen
           RUNTIME DESTINATION bin )


