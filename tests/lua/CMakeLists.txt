cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

add_custom_target( LuaTestExecDir ALL COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/tests/lua/exec" )

add_test( LuaHelloWorld ${CMAKE_BINARY_DIR}/src/lang/lua/strusLua  "${PROJECT_SOURCE_DIR}/tests/lua/testHelloWorld.lua" )
add_test( LuaCreateCollection_t3s ${CMAKE_BINARY_DIR}/src/lang/lua/strusLua  "${PROJECT_SOURCE_DIR}/tests/lua/testCreateCollection_t3s.lua"  "${PROJECT_SOURCE_DIR}/tests/data/t3s" "${CMAKE_BINARY_DIR}/tests/lua/exec" )
add_test( LuaQuery_t3s ${CMAKE_BINARY_DIR}/src/lang/lua/strusLua  "${PROJECT_SOURCE_DIR}/tests/lua/testQuery_t3s.lua"  "${PROJECT_SOURCE_DIR}/tests/data/t3s" "${CMAKE_BINARY_DIR}/tests/lua/exec" )
add_test( LuaCreateCollection_mdprim ${CMAKE_BINARY_DIR}/src/lang/lua/strusLua  "${PROJECT_SOURCE_DIR}/tests/lua/testCreateCollection_mdprim.lua"  "${PROJECT_SOURCE_DIR}/tests/data/mdprim" "${CMAKE_BINARY_DIR}/tests/lua/exec" )
add_test( LuaQuery_mdprim ${CMAKE_BINARY_DIR}/src/lang/lua/strusLua  "${PROJECT_SOURCE_DIR}/tests/lua/testQuery_mdprim.lua"  "${PROJECT_SOURCE_DIR}/tests/data/mdprim" "${CMAKE_BINARY_DIR}/tests/lua/exec" )
add_test( LuaVectors ${CMAKE_BINARY_DIR}/src/lang/lua/strusLua  "${PROJECT_SOURCE_DIR}/tests/lua/testVectors.lua"  "${CMAKE_BINARY_DIR}/tests/lua/exec" )



