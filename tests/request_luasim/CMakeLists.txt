cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

SET( TEST_REQUEST_LUASIM_RUN_DIR "${CMAKE_CURRENT_BINARY_DIR}" )
SET( TEST_REQUEST_LUASIM_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}" )

IF (WITH_STRUS_PATTERN STREQUAL "YES")
SET( STRUS_MODULES_LOADED "${STRUS_MODULES_LOADED}${STRUS_MODULES_LOADED_DELIMITER}\"modstrus_analyzer_pattern\"" )
SET( STRUS_MODULES_LOADED_DELIMITER "," )
ENDIF (WITH_STRUS_PATTERN STREQUAL "YES")

IF (WITH_STRUS_VECTOR STREQUAL "YES")
SET( STRUS_MODULES_LOADED "${STRUS_MODULES_LOADED}${STRUS_MODULES_LOADED_DELIMITER}\"modstrus_storage_vector_std\"" )
SET( STRUS_MODULES_LOADED_DELIMITER "," )
ENDIF (WITH_STRUS_VECTOR STREQUAL "YES")

configure_file( "${CMAKE_CURRENT_LIST_DIR}/config.lua.in"   "${CMAKE_CURRENT_BINARY_DIR}/config.lua"   @ONLY )

set( WS_CMD_BINARY_DIR ${PROJECT_BINARY_DIR}/src  )
STRING( REGEX REPLACE "strusBindings" "strusWebService" WS_CMD_BINARY_DIR ${WS_CMD_BINARY_DIR})
set( WS_CMD_PRG "${WS_CMD_BINARY_DIR}/strusWebService --config {config} --port {port}" )

add_custom_target( test_ws )
function( DeclareTest TestTitle TestScript Argument )
	set( LuaSim ${REQUEST_LUASIM_BUILD_DIR}/strusWebRequestLuaSim )
	add_test( ${TestTitle}  ${LuaSim} ${CMAKE_CURRENT_LIST_DIR}/${TestScript}  ${CMAKE_CURRENT_BINARY_DIR} ${Argument} )
	IF (Argument STREQUAL "")
		set( TestWsCommand  "${LuaSim} --program '${WS_CMD_PRG}' ${CMAKE_CURRENT_LIST_DIR}/${TestScript}  ${CMAKE_CURRENT_BINARY_DIR}" )
	ELSE (Argument STREQUAL "")
		set( TestWsCommand  "${LuaSim} --program '${WS_CMD_PRG}' ${CMAKE_CURRENT_LIST_DIR}/${TestScript}  ${CMAKE_CURRENT_BINARY_DIR}  ${Argument}" )
	ENDIF (Argument STREQUAL "")
	add_custom_command( 
		TARGET test_ws
		COMMENT "Running test ${TestTitle}:\n${TestWsCommand}"
		COMMAND  ${LuaSim} --program "${WS_CMD_PRG}" ${CMAKE_CURRENT_LIST_DIR}/${TestScript}  ${CMAKE_CURRENT_BINARY_DIR}  ${Argument} )
endfunction( DeclareTest )

DeclareTest( MapLuaTableToJson configToJson.lua "" )
DeclareTest( EmbeddedConfig  embeddedConfig.lua "" )
DeclareTest( DocumentStructureAnalysis documentStructureAnalysis.lua "" )
DeclareTest( DocumentAnalysis docanalyzer.lua "" )
DeclareTest( QueryAnalysis qryanalyzer.lua "" )
DeclareTest( CreateStorage createStorage.lua "" )
DeclareTest( Query query.lua "" )
# TEMPORARILY DEPRECATED TESTS
# DeclareTest( DistributeStorage distributeStorage.lua "" )
# DeclareTest( DistributeStorageOneServer distributeStorage.lua "1server" )

IF (WITH_STRUS_VECTOR STREQUAL "YES")
# DeclareTest( WikipediaCase wikipedia.lua "" )
# DeclareTest( WikipediaCaseOneServer wikipedia.lua "1server" )
ENDIF (WITH_STRUS_VECTOR STREQUAL "YES")

