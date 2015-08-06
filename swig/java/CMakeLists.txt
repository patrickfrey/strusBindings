cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

include_directories( "${PROJECT_SOURCE_DIR}/include"  "${PROJECT_SOURCE_DIR}/swig/php"  ${JNI_INCLUDE_DIRS} ${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2} )
link_directories( "${PROJECT_SOURCE_DIR}/src" "${strusmodule_LIBRARY_DIRS}" "${strusrpc_LIBRARY_DIRS}" )

# Java:
add_custom_command(
	OUTPUT strus_wrap.c ../strus.java java_strus.h Aggregator.java strusJNI.java Attribute.java SummarizerConfig.java DocumentAnalyzer.java SWIGTYPE_p_std__vectorT_Attribute_t.java DocumentClass.java  SWIGTYPE_p_std__vectorT_MetaData_t.java Document.java SWIGTYPE_p_std__vectorT_Normalizer_t.java MetaData.java SWIGTYPE_p_std__vectorT_RankAttribute_t.java Normalizer.java SWIGTYPE_p_std__vectorT_Rank_t.java QueryAnalyzeQueue.java SWIGTYPE_p_std__vectorT_std__string_t.java QueryAnalyzer.java SWIGTYPE_p_std__vectorT_Term_t.java QueryEval.java  Term.java Query.java Tokenizer.java RankAttribute.java Variant.java Rank.java VariantType.java StorageClient.java VariantValue.java StrusContext.java WeightingConfig.java strus.java
	PRE_BUILD
	COMMAND  swig -Werror -java -c++ -outdir ${PROJECT_SOURCE_DIR}/swig/java -o strus_wrap.cxx strus.i
	WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/swig/java"
	COMMENT "build Java bindings" VERBATIM
	DEPENDS  strus.i exception_handler.i ../ignore_declarations.i
#	DEPENDS  strus.i strus_variant.i strus_tokenizer.i strus_normalizer.i strus_aggregator.i std_vector_string.i std_vector_rank.i std_vector_term.i exception_handler.i   strus_weighting.i strus_summarizer.i  ../ignore_declarations.i
)

add_library( strus_java MODULE strus_wrap.cxx )
set_target_properties( strus_java PROPERTIES PREFIX "")
target_link_libraries( strus_java  strus_bindings ${JNI_LIBRARIES} )

# ------------------------------
# INSTALLATION
# ------------------------------


