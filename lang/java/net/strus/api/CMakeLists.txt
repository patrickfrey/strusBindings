cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

set( generated_java_source_files
	Aggregator.java
	Attribute.java
	AttributeVector.java
	DocumentAnalyzer.java
	DocumentAnalyzeQueue.java
	DocumentClass.java
	Document.java
	MetaData.java
	MetaDataVector.java
	Normalizer.java
	NormalizerVector.java
	QueryAnalyzeQueue.java
	QueryAnalyzer.java
	QueryEval.java
	Query.java
	QueryExpression.java
	RankAttribute.java
	RankAttributeVector.java
	Rank.java
	RankVector.java
	StorageClient.java
	StringVector.java
	Context.java
	strus.java
	strusJNI.java
	SummarizerConfig.java
	Term.java
	TermVector.java
	Tokenizer.java
	Variant.java
	VariantType.java
	VariantValue.java
	WeightingConfig.java
)
set( CMAKE_JAVA_TARGET_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/lang/java/net/strus/api )
add_jar( strus_api ${generated_java_source_files} OUTPUT_DIR ${PROJECT_SOURCE_DIR}/lang/java/net/strus/api)

# ------------------------------
# INSTALLATION
# ------------------------------
install_jar( strus_api ${JAVA_LIB_INSTALL_DIR}/strus )


