cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

add_test( LuaCreateCollection ${CMAKE_BINARY_DIR}/src/lang/lua/strusLua  "${PROJECT_SOURCE_DIR}/tests/lua/create_collection_1/run.lua"  "${PROJECT_SOURCE_DIR}/tests/data/t3s" )



