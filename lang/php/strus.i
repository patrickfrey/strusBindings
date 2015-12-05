%module strus
%include "std_string.i"
%include "std_vector.i"
%include "typemaps.i"
%include "exception.i"

%apply const std::string& {std::string*};

%{
#include "../include/strus/bindingObjects.hpp"
%}

%rename(StrusGlobalStatistics) GlobalStatistics;
%rename(StrusTermStatistics) TermStatistics;
%rename(TokenNormalizer) Normalizer;
%rename(StrusTokenizer) Tokenizer;
%rename(StrusNormalizer) Normalizer;
%rename(StrusAggregator) Aggregator;
%rename(StrusVariant) Variant;
%rename(StrusTerm) Term;
%rename(StrusMetaData) MetaData;
%rename(StrusAttribute) Attribute;
%rename(StrusDocumentClass) DocumentClass;
%rename(StrusDocument) Document;
%rename(StrusDocumentAnalyzer) DocumentAnalyzer;
%rename(StrusQueryAnalyzeQueue) QueryAnalyzeQueue;
%rename(StrusQueryAnalyzer) QueryAnalyzer;
%rename(StrusQueryAnalyzeQueue) QueryAnalyzeQueue;
%rename(StrusStorageClient) StorageClient;
%rename(StrusStorageTransaction) StorageTransaction;
%rename(StrusPeerStorageTransaction) PeerStorageTransaction;
%rename(StrusPeerMessage) PeerMessage;
%rename(StrusPeerMessageIterator) PeerMessageIterator;
%rename(StrusSummarizerConfig) SummarizerConfig;
%rename(StrusWeightingConfig) WeightingConfig;
%rename(StrusQueryExpression) QueryExpression;
%rename(StrusQuery) Query;
%rename(StrusQueryEval) QueryEval;
%rename(StrusRankAttribute) RankAttribute;
%rename(StrusRank) Rank;
%rename(StrusQuery) Query;
%rename(StrusContext) Context;

%include "ignore_declarations.i"
%include "exception_handler.i"
%include "std_vector_string.i"
%include "std_vector_int.i"
%include "std_vector_term.i"
%include "std_vector_rank.i"
%include "strus_variant.i"
%include "strus_tokenizer.i"
%include "strus_normalizer.i"
%include "strus_aggregator.i"
%include "strus_summarizer.i"
%include "strus_weighting.i"
%include "strus_queryexpr.i"
%include "strus_statistics.i"
%include "../../include/strus/bindingObjects.hpp"




