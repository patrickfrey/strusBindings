cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

include_directories(
	${Boost_INCLUDE_DIRS}
	"${Intl_INCLUDE_DIRS}"
	"${PROJECT_SOURCE_DIR}/include"
	"${strusbase_INCLUDE_DIRS}"
	${REQUEST_SOURCE_DIRS}
)

link_directories(
	${REQUEST_SOURCE_DIRS}
	${Boost_LIBRARY_DIRS}
	"${strusbase_LIBRARY_DIRS}"
)

add_executable( testRequestParseHttpAccept  testRequestParseHttpAccept.cpp)
target_link_libraries( testRequestParseHttpAccept strus_webrequest_static  strus_base ${Boost_LIBRARIES} ${Intl_LIBRARIES} )


