cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

include_directories( "${PROJECT_SOURCE_DIR}/include"  ${PYTHON_INCLUDE_DIRS} )
link_directories( "${PROJECT_SOURCE_DIR}/src" "${strusmodule_LIBRARY_DIRS}" "${strusrpc_LIBRARY_DIRS}" )

# Python:
add_custom_command(
	OUTPUT strus_wrap.cxx python_strus.h
	PRE_BUILD
	COMMAND  swig -Werror -python -c++ -outdir ${PROJECT_SOURCE_DIR}/swig/python -o strus_wrap.cxx strus.i
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/swig/python"
	COMMENT "build Python bindings" VERBATIM
	DEPENDS  strus.i exception_handler.i ../ignore_declarations.i
#	DEPENDS  strus.i strus_variant.i strus_tokenizer.i strus_normalizer.i strus_aggregator.i std_vector_string.i std_vector_rank.i std_vector_term.i exception_handler.i   strus_weighting.i strus_summarizer.i  ../ignore_declarations.i
)

add_library( strus_python MODULE strus_wrap.cxx )
set_target_properties( strus_python PROPERTIES PREFIX "")
target_link_libraries( strus_python strus_bindings ${PYTHON_LIBRARIES} )

# ------------------------------
# INSTALLATION
# ------------------------------


