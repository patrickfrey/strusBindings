cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

set( generated_java_source_files
	StringVector.java
	Aggregator.java
	Attribute.java
	AttributeVector.java
	VecRank.java
	VecRankVector.java
	DocumentAnalyzer.java
	DocumentAnalyzeQueue.java
	DocumentClass.java
	Document.java
	DocumentFrequencyChange.java
	DocumentFrequencyChangeVector.java
	MetaData.java
	MetaDataVector.java
	Normalizer.java
	NormalizerVector.java
	QueryAnalyzeContext.java
	QueryAnalyzer.java
	PatternMatcher.java
	QueryEval.java
	Query.java
	QueryExpression.java
	QueryResult.java
	DocumentBrowser.java
	TermStatistics.java
	GlobalStatistics.java
	SummaryElement.java
	SummaryElementVector.java
	Rank.java
	RankVector.java
	StorageClient.java
	StorageTransaction.java
	VectorStorageClient.java
	VectorStorageSearcher.java
	VectorStorageTransaction.java
	StatisticsIterator.java
	StatisticsProcessor.java
	StatisticsMessage.java
	Context.java
	strus.java
	strusJNI.java
	SummarizerConfig.java
	Term.java
	TermVector.java
	QueryTerm.java
	QueryTermVector.java
	Tokenizer.java
	Variant.java
	VariantType.java
	VariantValue.java
	WeightingConfig.java
	FunctionVariableConfig.java
)
set( CMAKE_JAVA_TARGET_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/lang/java/net/strus/api )
add_jar( strus_api ${generated_java_source_files} OUTPUT_DIR ${PROJECT_SOURCE_DIR}/lang/java/net/strus/api)

# ------------------------------
# INSTALLATION
# ------------------------------
install_jar( strus_api ${JAVA_LIB_INSTALL_DIR}/strus )


