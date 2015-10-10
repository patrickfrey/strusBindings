cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-strict-aliasing" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing" )

include_directories( "${PROJECT_SOURCE_DIR}/include"  ${PYTHON_INCLUDE_DIRS} )
link_directories( "${PROJECT_SOURCE_DIR}/src" "${strusmodule_LIBRARY_DIRS}" "${strusrpc_LIBRARY_DIRS}" )

# Python:
add_custom_command(
	OUTPUT strus_wrap.cxx python_strus.h
	PRE_BUILD
	COMMAND  swig -Werror -python -c++ -outdir ${PROJECT_SOURCE_DIR}/swig/python -o strus_wrap.cxx strus.i
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/swig/python"
	COMMENT "build Python bindings" VERBATIM
	DEPENDS  strus.i exception_handler.i strus_variant.i strus_tokenizer.i strus_normalizer.i strus_aggregator.i strus_summarizer.i  strus_weighting.i  std_vector_string.i std_vector_int.i std_vector_rank.i std_vector_term.i ignore_declarations.i
)

add_library( _strus MODULE strus_wrap.cxx objInitializers.cpp )
set_target_properties( _strus PROPERTIES PREFIX "")
target_link_libraries( _strus strus_bindings ${PYTHON_LIBRARIES} )

add_subdirectory( example )

# ------------------------------
# INSTALLATION
# ------------------------------
install( TARGETS _strus
           LIBRARY DESTINATION ${LIB_INSTALL_DIR}/strus/python )

