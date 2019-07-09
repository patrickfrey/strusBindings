cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

configure_file( "${CMAKE_CURRENT_LIST_DIR}/config.lua.in"   "${CMAKE_CURRENT_LIST_DIR}/config.lua"   @ONLY )

add_test( DocumentStructureAnalysis ${REQUEST_LUASIM_BUILD_DIR}/strusWebRequestLuaSim ${CMAKE_CURRENT_LIST_DIR}/documentStructureAnalysis.lua )

