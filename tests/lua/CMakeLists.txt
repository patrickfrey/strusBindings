cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

add_custom_target( LuaTestExecDir ALL COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/tests/lua/exec" )

add_test( LuaHelloWorld ${CMAKE_BINARY_DIR}/src/lang/lua/strusLua  "${PROJECT_SOURCE_DIR}/tests/lua/testHelloWorld.lua" )
add_test( LuaCreateCollection ${CMAKE_BINARY_DIR}/src/lang/lua/strusLua  "${PROJECT_SOURCE_DIR}/tests/lua/testCreateCollection1.lua"  "${PROJECT_SOURCE_DIR}/tests/data/t3s" "${CMAKE_BINARY_DIR}/tests/lua/exec" )



