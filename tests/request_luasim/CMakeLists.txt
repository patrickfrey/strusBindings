cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

SET( TEST_REQUEST_LUASIM_RUN_DIR "${CMAKE_CURRENT_BINARY_DIR}" )
IF (WITH_STRUS_PATTERN STREQUAL "YES")
SET( STRUS_MODULES_LOADED "${STRUS_MODULES_LOADED}${STRUS_MODULES_LOADED_DELIMITER}\"modstrus_analyzer_pattern\"" )
SET( STRUS_MODULES_LOADED_DELIMITER "," )
ENDIF (WITH_STRUS_PATTERN STREQUAL "YES")

IF (WITH_STRUS_VECTOR STREQUAL "YES")
SET( STRUS_MODULES_LOADED "${STRUS_MODULES_LOADED}${STRUS_MODULES_LOADED_DELIMITER}\"modstrus_storage_vector_std\"" )
SET( STRUS_MODULES_LOADED_DELIMITER "," )
ENDIF (WITH_STRUS_VECTOR STREQUAL "YES")

configure_file( "${CMAKE_CURRENT_LIST_DIR}/config.lua.in"   "${CMAKE_CURRENT_BINARY_DIR}/config.lua"   @ONLY )

add_test( DocumentStructureAnalysis  ${REQUEST_LUASIM_BUILD_DIR}/strusWebRequestLuaSim ${CMAKE_CURRENT_LIST_DIR}/documentStructureAnalysis.lua  ${CMAKE_CURRENT_BINARY_DIR} )
