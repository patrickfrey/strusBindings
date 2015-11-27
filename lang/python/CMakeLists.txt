cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

include_directories(
	"${PROJECT_SOURCE_DIR}/include" 
	${PYTHON_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
)

link_directories(
        "${Boost_LIBRARY_DIRS}"
	"${PROJECT_SOURCE_DIR}/src"
	"${strusmodule_LIBRARY_DIRS}"
	"${strusrpc_LIBRARY_DIRS}"
)
add_library( strus_python MODULE strusPythonModule.cpp bindingObjectsExt.cpp objInitializers.cpp )
set_target_properties( strus_python PROPERTIES PREFIX "" OUTPUT_NAME strus)
target_link_libraries( strus_python strus_bindings ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${Boost_Python_LIBRARIES} )

add_subdirectory( example )

# ------------------------------
# INSTALLATION
# ------------------------------
install( TARGETS strus_python
           LIBRARY DESTINATION ${PYTHON_SITE_PACKAGES} )



