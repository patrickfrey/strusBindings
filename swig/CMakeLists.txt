cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

include_directories( "${Boost_INCLUDE_DIRS}"  "${PROJECT_SOURCE_DIR}/include"  "${strus_INCLUDE_DIRS}" "${strusanalyzer_INCLUDE_DIRS}" )
link_directories( "${Boost_LIBRARY_DIRS}" "${PROJECT_SOURCE_DIR}/src"  "${strus_LIBRARY_DIRS}"  "${strusanalyzer_LIBRARY_DIRS}" )

find_package( SWIG REQUIRED )
include( ${SWIG_USE_FILE} )

find_package( PythonLibs QUIET )
IF (${PYTHONLIBS_FOUND})
include_directories( ${PYTHON_INCLUDE_PATH} )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

set( CMAKE_SWIG_FLAGS "" )

set_source_files_properties( strus.i PROPERTIES CPLUSPLUS ON)
swig_add_module( strus python strus.i )
swig_link_libraries( strus ${PYTHON_LIBRARIES} strus_bindings)
ELSE (${PYTHONLIBS_FOUND})
Message( "PythonLibs not found. Skipping generation of python bindings" )
ENDIF (${PYTHONLIBS_FOUND})

