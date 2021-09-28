cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

include_directories(
	${Boost_INCLUDE_DIRS}
	"${Intl_INCLUDE_DIRS}"
	${PAPUGA_INCLUDE_DIRS}
	"${PROJECT_SOURCE_DIR}/include"
	"${strusbase_INCLUDE_DIRS}"
	${REQUEST_SOURCE_DIRS}
	${BINDINGS_INCLUDE_DIRS}
)

link_directories(
	${REQUEST_SOURCE_DIRS}
	${PAPUGA_LIBRARY_DIRS}
	${Boost_LIBRARY_DIRS}
	"${strusbase_LIBRARY_DIRS}"
)

add_executable( testSchemaItemNameMap  testSchemaItemNameMap.cpp)
target_link_libraries( testSchemaItemNameMap strus_webrequest_static  strus_base ${Boost_LIBRARIES} ${Intl_LIBRARIES} )


