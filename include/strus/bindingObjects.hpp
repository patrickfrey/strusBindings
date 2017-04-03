/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_OBJECTS_HPP_INCLUDED
#define _STRUS_BINDING_OBJECTS_HPP_INCLUDED
#include "strus/bindings/hostObjectReference.hpp"
#include "strus/bindings/valueVariant.hpp"
#include "strus/bindings/statisticsMessage.hpp"
#include "strus/numericVariant.hpp"
#include "strus/analyzer/document.hpp"
#include "strus/textProcessorInterface.hpp"

/// \brief Forward declaration
class DocumentAnalyzeQueue;
/// \brief Forward declaration
class PatternMatcher;

namespace strus {
namespace bindings {

/// \class DocumentAnalyzer
/// \brief Analyzer object representing a program for segmenting, 
///	tokenizing and normalizing a document into atomic parts, that 
///	can be inserted into a storage and be retrieved from there.
/// \remark The only way to construct a document analyzer instance is to call Context::createDocumentAnalyzer()
class DocumentAnalyzer
{
public:
	/// \brief Copy constructor
	DocumentAnalyzer( const DocumentAnalyzer& o);
	/// \brief Destructor
	virtual ~DocumentAnalyzer(){}

	/// \brief Define a feature to insert into the inverted index (search index) is selected, tokenized and normalized
	/// \param[in] type type of the features produced
	/// \param[in] selectexpr expression selecting the elements to fetch for producing this feature
	/// \param[in] tokenizer tokenizer function description to use for this feature
	/// \param[in] normalizers list of normalizer function description to use for this feature in the ascending order of appearance
	/// \param[in] options a list of option strings, one of {"content" => feature has own position, "unique" => feature gets position but sequences or "unique" features without "content" features in between are mapped to one position, "pred" => the position is bound to the preceeding feature, "succ" => the position is bound to the succeeding feature}
	void addSearchIndexFeature(
		const std::string& type,
		const std::string& selectexpr,
		const ValueVariant& tokenizer,
		const ValueVariant& normalizers,
		const ValueVariant& options);

	/// \brief Define a feature to insert into the forward index (for summarization) is selected, tokenized and normalized
	/// \param[in] type type of the features produced
	/// \param[in] selectexpr expression selecting the elements to fetch for producing this feature
	/// \param[in] tokenizer tokenizer function description to use for this feature
	/// \param[in] normalizers list of normalizer function description to use for this feature in the ascending order of appearance
	/// \param[in] options a list of option strings, one of {"content" => feature has own position, "unique" => feature gets position but sequences or "unique" features without "content" features in between are mapped to one position, "pred" => the position is bound to the preceeding feature, "succ" => the position is bound to the succeeding feature}
	void addForwardIndexFeature(
		const std::string& type,
		const std::string& selectexpr,
		const ValueVariant& tokenizer,
		const ValueVariant& normalizers,
		const ValueVariant& options);

	/// \brief Define a feature to insert as meta data is selected, tokenized and normalized
	/// \param[in] fieldname name of the addressed meta data field.
	/// \param[in] selectexpr expression selecting the elements to fetch for producing this feature
	/// \param[in] tokenizer tokenizer function description to use for this feature
	/// \param[in] normalizers list of normalizer function description to use for this feature in the ascending order of appearance
	void defineMetaData(
		const std::string& fieldname,
		const std::string& selectexpr,
		const ValueVariant& tokenizer,
		const ValueVariant& normalizers);

	/// \brief Declare some aggregated value of the document to be put into the meta data table used for restrictions, weighting and summarization.
 	/// \param[in] fieldname name of the addressed meta data field.
	/// \param[in] function defining how and from what the value is aggregated
	void defineAggregatedMetaData(
		const std::string& fieldname,
		const ValueVariant& function);

	/// \brief Define a feature to insert as document attribute (for summarization) is selected, tokenized and normalized
	/// \param[in] attribname name of the addressed attribute.
	/// \param[in] selectexpr expression selecting the elements to fetch for producing this feature
	/// \param[in] tokenizer tokenizer function description to use for this feature
	/// \param[in] normalizers list of normalizer function description to use for this feature in the ascending order of appearance
	void defineAttribute(
		const std::string& attribname,
		const std::string& selectexpr,
		const ValueVariant& tokenizer,
		const ValueVariant& normalizers);

	/// \brief Define a result of pattern matching as feature to insert into the search index, normalized
	/// \param[in] type type name of the feature to produce.
	/// \param[in] patternTypeName name of the pattern to select
	/// \param[in] normalizers list of normalizer function description to use for this feature in the ascending order of appearance
	/// \param[in] options a list of option strings, one of {"content" => feature has own position, "unique" => feature gets position but sequences or "unique" features without "content" features in between are mapped to one position, "pred" => the position is bound to the preceeding feature, "succ" => the position is bound to the succeeding feature}
	void addSearchIndexFeatureFromPatternMatch(
		const std::string& type,
		const std::string& patternTypeName,
		const ValueVariant& normalizers,
		const ValueVariant& options);

	/// \brief Define a result of pattern matching as feature to insert into the forward index, normalized
	/// \param[in] type type name of the feature to produce.
	/// \param[in] patternTypeName name of the pattern to select
	/// \param[in] normalizers list of normalizer function description to use for this feature in the ascending order of appearance
	/// \param[in] options a list of options as std::string, elements one of {"BindPosPred" => the position is bound to the preceeding feature, "BindPosSucc" => the position is bound to the succeeding feature}
	void addForwardIndexFeatureFromPatternMatch(
		const std::string& type,
		const std::string& patternTypeName,
		const ValueVariant& normalizers,
		const ValueVariant& options);

	/// \brief Define a result of pattern matching to insert as metadata, normalized
	/// \param[in] fieldname field name of the meta data element to produce.
	/// \param[in] patternTypeName name of the pattern to select
	/// \param[in] normalizers list of normalizer function description to use for this feature in the ascending order of appearance
	void defineMetaDataFromPatternMatch(
		const std::string& fieldname,
		const std::string& patternTypeName,
		const ValueVariant& normalizers);

	/// \brief Define a result of pattern matching to insert as document attribute, normalized
	/// \param[in] attribname name of the document attribute to produce.
	/// \param[in] patternTypeName name of the pattern to select
	/// \param[in] normalizers list of normalizer function description to use for this feature in the ascending order of appearance
	void defineAttributeFromPatternMatch(
		const std::string& attribname,
		const std::string& patternTypeName,
		const ValueVariant& normalizers);

	/// \brief Declare a pattern matcher on the document features after other query analysis
	/// \param[in] patternTypeName name of the type to assign to the pattern matching results
	/// \param[in] patternMatcherModule module id of pattern matcher to use (empty std::string for default)
	/// \param[in] patterns structure with all patterns
	void definePatternMatcherPostProc(
			const std::string& patternTypeName,
			const std::string& patternMatcherModule,
			const ValueVariant& patterns);

	/// \brief Declare a pattern matcher on the document features after other query analysis
	/// \param[in] patternTypeName name of the type to assign to the pattern matching results
	/// \param[in] patternMatcherModule module id of pattern matcher to use (empty std::string for default)
	/// \param[in] serializedPatternFile path to file with serialized (binary) patterns
	void definePatternMatcherPostProcFromFile(
			const std::string& patternTypeName,
			const std::string& patternMatcherModule,
			const std::string& serializedPatternFile);

	/// \brief Declare a sub document for the handling of multi part documents in an analyzed content
	/// \param[in] selectexpr an expression that defines the content of the sub document declared
	/// \param[in] subDocumentTypeName type name assinged to this sub document
	/// \remark Sub documents are defined as the sections selected by the expression plus some data selected not belonging to any sub document.
	void defineDocument(
			const std::string& subDocumentTypeName,
			const std::string& selectexpr);

	/// \brief Analye the content and return the set of features to insert
	/// \param[in] content std::string (NOT a file name !) of the document to analyze
	/// \return structure of the document analyzed (sub document type names, search index terms, forward index terms, metadata, attributes) (See analyzer::Document)
	ValueVariant analyze(
			const std::string& content);

	/// \brief Analye the content and return the set of features to insert
	/// \param[in] content std::string (NOT a file name !) of the document to analyze
	/// \param[in] documentClass document class of the document to analyze (see analyzer::DocumentClass)
	/// \return structure of the document analyzed (sub document type names, search index terms, forward index terms, metadata, attributes) (See analyzer::Document)
	ValueVariant analyze(
			const std::string& content,
			const ValueVariant& documentClass);

	/// \brief Creates a queue for multi document analysis
	/// \return the queue
	DocumentAnalyzeQueue createQueue() const;

private:
	/// \brief Constructor used by Context
	friend class Context;
	DocumentAnalyzer( const HostObjectReference& objbuilder, const HostObjectReference& trace, const HostObjectReference& errorhnd, const std::string& segmentername, const void* textproc_);

	HostObjectReference m_errorhnd_impl;
	HostObjectReference m_trace_impl;
	HostObjectReference m_objbuilder_impl;
	HostObjectReference m_analyzer_impl;
	const TextProcessorInterface* m_textproc;
};


/// \class DocumentAnalyzeQueue
/// \brief Analyzer object implementing a queue of analyze document tasks.
/// \remark Document analysis with this class reduces network roundtrips when using it as proxy
class DocumentAnalyzeQueue
{
public:
	/// \brief Copy constructor
	DocumentAnalyzeQueue( const DocumentAnalyzeQueue& o);
	/// \brief Destructor
	virtual ~DocumentAnalyzeQueue(){}

	/// \brief Push a document into the queue to analyze
	/// \param[in] content content std::string of the document to analyze
	void push( const std::string& content);

	/// \brief Push a document into the queue to analyze
	/// \param[in] content content std::string of the document to analyze
	/// \param[in] documentClass document class of the document to analyze
	void push( const std::string& content, const ValueVariant& documentClass);

	/// \brief Checks if there are more results to fetch
	/// \return true, if yes
	bool hasMore() const;

	/// \brief Processes the next field of the queue for fields to analyzer. Does the tokenization and normalization and creates some typed terms out of it according the definition of the field type given.
	/// \return document structure with sub document type, search index terms, forward index terms, metadata, attributes
	ValueVariant fetch();

private:
	void analyzeNext();

private:
	/// \brief Constructor used by Context
	friend class DocumentAnalyzer;
	explicit DocumentAnalyzeQueue( const HostObjectReference& objbuilder, const HostObjectReference& trace, const HostObjectReference& errorhnd, const HostObjectReference& analyzer, const void* textproc_);

	HostObjectReference m_errorhnd_impl;
	HostObjectReference m_trace_impl;
	HostObjectReference m_objbuilder_impl;
	HostObjectReference m_analyzer_impl;
	std::vector<analyzer::Document> m_result_queue;
	std::size_t m_result_queue_idx;
	std::vector<HostObjectReference> m_analyzerctx_queue;
	std::size_t m_analyzerctx_queue_idx;
	const TextProcessorInterface* m_textproc;
};


/// \brief Forward declaration
class QueryAnalyzeContext;

/// \brief Object representing a pattern match program
class PatternMatcher
{
public:
	/// \brief Copy constructor
	PatternMatcher( const PatternMatcher& o)
		:m_ops(o.m_ops),m_strings(o.m_strings),m_size(o.m_size){}
	/// \brief Default constructor
	PatternMatcher()
		:m_size(0){}
	/// \brief Destructor
	virtual ~PatternMatcher(){}

	/// \brief Add operation push term
	/// \param[in] type_ query term type name
	/// \param[in] value_ query term value
	void pushTerm( const std::string& type_, const std::string& value_);

	/// \brief Add operation push term
	/// \param[in] name_ name of the referenced pattern
	void pushPattern( const std::string& name_);

	/// \brief Add operation push expression
	/// \param[in] opname_ name of the expression operator
	/// \param[in] argc_ number of operands (topmost elements from stack) of the expression
	/// \param[in] range_ range number for the expression span in the document
	/// \param[in] cardinality_ number that specifies the minimum size of a subset or subset permutation to match
	void pushExpression( const std::string& opname_, unsigned int argc_, int range_=0, unsigned int cardinality_=0);

	/// \brief Add operation push pattern
	/// \param[in] name_ name of the pattern created
	void definePattern( const std::string& name_, bool visible);

	/// \brief Add operation attach variable
	/// \note The positions of the query matches of the referenced term or expression can be accessed through this variable in summarization.
	/// \param[in] name_ name of the variable attached
	/// \remark The stack is not changed
	void attachVariable( const std::string& name_);

	/// \brief Appends the operations of 'o' to this
	/// \param[in] o expression to copy
	void add( const PatternMatcher& o);

	/// \brief Get the number of items (sub expressions) on the stack as result of this expression
	/// \return the number of items (sub expressions)
	std::size_t size() const
	{
		return m_size;
	}

private:
	friend class Query;

	struct StackOp
	{
		enum Type
		{
			PushTerm,
			PushPattern,
			PushExpression,
			DefinePattern,
			AttachVariable
		};
		enum ArgIndex
		{
			Term_type=0x0,
			Term_value=0x1,
			Expression_opname=0x0,
			Expression_argc=0x1,
			Expression_range=0x2,
			Expression_cardinality=0x3,
			Variable_name=0x0,
			Pattern_name=0x0,
			Pattern_visible=0x1
		};
		Type type;
		int arg[4];

		StackOp( Type type_, int arg0_, int arg1_=0, int arg2_=0, int arg3_=0)
			:type(type_)
		{
			arg[0] = arg0_;
			arg[1] = arg1_;
			arg[2] = arg2_;
			arg[3] = arg3_;
		}

		StackOp()
			:type(PushTerm)
		{
			std::memset( arg, 0, sizeof(arg));
		}
		StackOp( const StackOp& o)
		{
			std::memcpy( this, &o, sizeof(*this));
		}
	};
	const std::vector<StackOp>& ops() const
	{
		return m_ops;
	}
	const std::string& strings() const
	{
		return m_strings;
	}

private:
	std::size_t allocid( const std::string& str);

private:
	friend class QueryAnalyzer;
	friend class DocumentAnalyzer;
	friend class PatternMatchLoader;
	std::vector<StackOp> m_ops;
	std::string m_strings;
	std::size_t m_size;
};


/// \class QueryAnalyzer
/// \brief Analyzer object representing a set of function for transforming a field,
///	the smallest unit in any query language, to a set of terms that can be used
///	to build a query.
/// \remark The only way to construct a query analyzer instance is to call Context::createQueryAnalyzer()
class QueryAnalyzer
{
public:
	/// \brief Copy constructor
	QueryAnalyzer( const QueryAnalyzer& o);
	/// \brief Destructor
	virtual ~QueryAnalyzer(){}

	/// \brief Defines a search index element.
	/// \param[in] featureType element feature type created from this field type
	/// \param[in] fieldType name of the field type defined
	/// \param[in] tokenizer tokenizer function description to use for the features of this field type
	/// \param[in] normalizers list of normalizer function description to use for the features of this field type in the ascending order of appearance
	void addSearchIndexElement(
			const std::string& featureType,
			const std::string& fieldType,
			const ValueVariant& tokenizer,
			const ValueVariant& normalizers);
	/// \brief Defines a search index element from a pattern matching result.
	/// \param[in] type element type created from this pattern match result type
	/// \param[in] patternTypeName name of the pattern match result item
	/// \param[in] normalizers list of normalizer functions
	void addSearchIndexElementFromPatternMatch(
			const std::string& type,
			const std::string& patternTypeName,
			const ValueVariant& normalizers);

	/// \brief Declare an element to be used as lexem by post processing pattern matching but not put into the result of query analysis
	/// \param[in] termtype term type name of the lexem to be feed to the pattern matching
	/// \param[in] fieldtype type of the field of this element in the query
	/// \param[in] tokenizer tokenizer function description to use for the features of this field type
	/// \param[in] normalizers list of normalizer function description to use for the features of this field type in the ascending order of appearance
	void addPatternLexem(
			const std::string& termtype,
			const std::string& fieldtype,
			const ValueVariant& tokenizer,
			const ValueVariant& normalizers);

	/// \brief Declare a pattern matcher on the query features after other query analysis
	/// \param[in] patternTypeName name of the type to assign to the pattern matching results
	/// \param[in] patternMatcherModule module id of pattern matcher to use (empty string for default)
	/// \param[in] patterns structure with all patterns
	void definePatternMatcherPostProc(
			const std::string& patternTypeName,
			const std::string& patternMatcherModule,
			const PatternMatcher& patterns);

	/// \brief Declare a pattern matcher on the query features after other query analysis
	/// \param[in] patternTypeName name of the type to assign to the pattern matching results
	/// \param[in] patternMatcherModule module id of pattern matcher to use (empty string for default)
	/// \param[in] serializedPatternFile path to file with serialized (binary) patterns
	void definePatternMatcherPostProcFromFile(
			const std::string& patternTypeName,
			const std::string& patternMatcherModule,
			const std::string& serializedPatternFile);

	/// \brief Tokenizes and normalizes a query field and creates some typed terms out of it according the definition of the field type given.
	/// \param[in] fieldType name of the field type to use for analysis
	/// \param[in] fieldContent content string of the query field to analyze
	/// \return list of terms in the query (std::vector<analyzer::Term>)
	/// \note This is a very simplistic method to analyze a query. For multi field queries the object QueryAnalyzeContext is more appropriate
	ValueVariant analyzeField(
			const std::string& fieldType,
			const std::string& fieldContent);

	/// \brief Creates a context for analyzing a multipart query
	/// \return the queue
	QueryAnalyzeContext createContext() const;

private:
	/// \brief Constructor used by Context
	friend class Context;
	QueryAnalyzer( const HostObjectReference& objbuilder, const HostObjectReference& trace, const HostObjectReference& errorhnd);

	HostObjectReference m_errorhnd_impl;
	HostObjectReference m_trace_impl;
	HostObjectReference m_objbuilder_impl;
	HostObjectReference m_analyzer_impl;
};

/// \class QueryAnalyzeContext
/// \brief Query analyzer context for analysing a multipart query.
class QueryAnalyzeContext
{
public:
	/// \brief Copy constructor
	QueryAnalyzeContext( const QueryAnalyzeContext& o);

	/// \brief Destructor
	virtual ~QueryAnalyzeContext(){}

	/// \brief Define a query field
	/// \param[in] fieldNo index given to the field by the caller, to identify its results
	/// \param[in] fieldType name of the field type to use for analysis
	/// \param[in] fieldContent content string of the query field to analyze
	void putField(
			unsigned int fieldNo, 
			const std::string& fieldType,
			const std::string& fieldContent);

	/// \brief Processes the next field of the queue for fields to analyzer. Does the tokenization and normalization and creates some typed terms out of it according the definition of the field type given.
	/// \return list of terms (analyzer::Query::Element)
	ValueVariant analyze();

private:
	/// \brief Constructor used by Context
	friend class QueryAnalyzer;
	explicit QueryAnalyzeContext( const HostObjectReference& objbuilder, const HostObjectReference& trace, const HostObjectReference& errorhnd, const HostObjectReference& analyzer);

	HostObjectReference m_errorhnd_impl;
	HostObjectReference m_trace_impl;
	HostObjectReference m_objbuilder_impl;
	HostObjectReference m_analyzer_impl;
	HostObjectReference m_analyzer_ctx_impl;
};

/// \brief Forward declaration
class StatisticsIterator;
/// \brief Forward declaration
class StorageTransaction;
/// \brief Forward declaration
class DocumentBrowser;

/// \brief Object representing a client connection to the storage 
/// \remark The only way to construct a storage client instance is to call Context::createStorageClient(const std::string&)
class StorageClient
{
public:
	/// \brief Copy constructor
	StorageClient( const StorageClient& o);

	/// \brief Destructor
	virtual ~StorageClient(){}

	/// \brief Get the number of documents inserted into the storage
	/// return the total number of documents
	int nofDocumentsInserted() const;

	/// \brief Create a transaction
	/// return the transaction object created
	StorageTransaction createTransaction() const;

	/// \brief Create an iterator on the storage statistics (total value) to distribute for initialization/deinitialization
	/// \param[in] sign true = registration, false = deregistration
	/// return the statistics iterator object created
	StatisticsIterator createInitStatisticsIterator( bool sign) const;

	/// \brief Create an iterator on the storage statistics (relative value) to distribute after storage updates
	/// return the statistics message iterator object created
	StatisticsIterator createUpdateStatisticsIterator() const;

	/// \brief Create a document browser instance
	DocumentBrowser createDocumentBrowser();

	/// \brief Close of the storage client
	void close();

private:
	friend class Context;
	StorageClient( const HostObjectReference& objbuilder, const HostObjectReference& trace, const HostObjectReference& errorhnd_, const std::string& config);

	friend class Query;
	friend class QueryEval;
	HostObjectReference m_errorhnd_impl;
	HostObjectReference m_trace_impl;
	HostObjectReference m_objbuilder_impl;
	HostObjectReference m_storage_impl;
};


/// \brief Object representing a transaction of the storage 
/// \remark The only way to construct a storage transaction instance is to call StorageClient::createTransaction()
class StorageTransaction
{
public:
	/// \brief Destructor
	virtual ~StorageTransaction(){}

	/// \brief Prepare the inserting a document into the storage
	/// \param[in] docid the identifier of the document to insert
	/// \param[in] doc the structure of the document to insert (analyzer::Document)
	/// \remark The document is physically inserted with the call of 'commit()'
	void insertDocument( const std::string& docid, const ValueVariant& doc);

	/// \brief Prepare the deletion of a document from the storage
	/// \param[in] docid the identifier of the document to delete
	/// \remark The document is physically deleted with the call of 'commit()'
	void deleteDocument( const std::string& docid);

	/// \brief Prepare the deletion of all document access rights of a user
	/// \param[in] username the name of the user to delete all access rights (in the local collection)
	/// \remark The user access rights are changed accordingly with the next implicit or explicit call of 'flush'
	void deleteUserAccessRights( const std::string& username);

	/// \brief Commit all insert or delete or user access right change statements of this transaction.
	void commit();

	/// \brief Rollback all insert or delete or user access right change statements of this transaction.
	void rollback();

private:
	friend class StorageClient;
	StorageTransaction( const HostObjectReference& objbuilder, const HostObjectReference& trace, const HostObjectReference& errorhnd_, const HostObjectReference& storage_);

	friend class Query;
	friend class QueryEval;
	HostObjectReference m_errorhnd_impl;
	HostObjectReference m_trace_impl;
	HostObjectReference m_objbuilder_impl;
	HostObjectReference m_storage_impl;
	HostObjectReference m_transaction_impl;
};

/// \brief Iterator on messages with storage statistics
class StatisticsIterator
{
public:
	/// \brief Destructor
	virtual ~StatisticsIterator(){}

	/// \brief Copy constructor
	StatisticsIterator( const StatisticsIterator& o);

	/// \brief Fetches the next statistics message
	/// \return message blob or empty string if there is no message left
	std::string getNext();

private:
	friend class StorageClient;
	StatisticsIterator( const HostObjectReference& objbuilder, const HostObjectReference& trace, const HostObjectReference& errorhnd_, const HostObjectReference& storage_, const HostObjectReference& iter_);

private:
	HostObjectReference m_errorhnd_impl;
	HostObjectReference m_trace_impl;
	HostObjectReference m_objbuilder_impl;
	HostObjectReference m_storage_impl;
	HostObjectReference m_iter_impl;
};


/// \brief Translation to update a storage with statistics messages
class StatisticsProcessor
{
public:
	/// \brief Destructor
	virtual ~StatisticsProcessor(){}

	/// \brief Decode a statistics message blob for introspection
	/// \param[in] blob statistics message blob
	/// \return the statistics message (bindings::StatisticsMessage)
	ValueVariant decode( const std::string& blob) const;
	/// \brief Create binary blob from statistics message
	/// \param[in] msg statistics message structure
	/// \return the statistics message blob
	std::string encode( const StatisticsMessage& msg) const;

private:
	friend class Context;
	StatisticsProcessor( const HostObjectReference& objbuilder_, const HostObjectReference& trace_, const std::string& name_, const HostObjectReference& errorhnd_);

private:
	HostObjectReference m_errorhnd_impl;
	HostObjectReference m_trace_impl;
	HostObjectReference m_objbuilder_impl;
	const void* m_statsproc;
};

class VectorStorageSearcher
{
public:
	/// \brief Copy constructor
	VectorStorageSearcher( const VectorStorageSearcher& o);

	/// \brief Destructor
	virtual ~VectorStorageSearcher(){}

	/// \brief Find the most similar vectors to vector
	/// \param[in] vec vector to search for (double[])
	/// \param[in] maxNofResults maximum number of results to return
	/// return the list of most similar vectors (double[])
	ValueVariant findSimilar( const ValueVariant& vec, unsigned int maxNofResults) const;

	/// \brief Find the most similar vectors to vector in a selection of features addressed by index
	/// \param[in] featidxlist list of candidate indices (int[])
	/// \param[in] vec vector to search for (double[])
	/// \param[in] maxNofResults maximum number of results to return
	/// return the list of most similar vectors (double[])
	ValueVariant findSimilarFromSelection( const ValueVariant& featidxlist, const ValueVariant& vec, unsigned int maxNofResults) const;

	/// \brief Controlled close to free resources (forcing free resources in interpreter context with garbage collector)
	void close();

private:
	friend class VectorStorageClient;
	VectorStorageSearcher( const HostObjectReference& storage, const HostObjectReference& trace, int range_from, int range_to, const HostObjectReference& errorhnd_);

	HostObjectReference m_errorhnd_impl;
	HostObjectReference m_searcher_impl;
	HostObjectReference m_trace_impl;
};

/// \brief Forward declaration
class VectorStorageTransaction;

/// \brief Object representing a client connection to a vector storage 
/// \remark The only way to construct a vector storage client instance is to call Context::createVectorStorageClient(const std::string&)
class VectorStorageClient
{
public:
	/// \brief Copy constructor
	VectorStorageClient( const VectorStorageClient& o);

	/// \brief Destructor
	virtual ~VectorStorageClient(){}

	/// \brief Create a searcher object for scanning the vectors for similarity
	/// \param[in] range_from start range of the features for the searcher (possibility to split into multiple searcher instances)
	/// \param[in] range_to end of range of the features for the searcher (possibility to split into multiple searcher instances)
	/// \return the vector search interface (with ownership)
	VectorStorageSearcher createSearcher( int range_from, int range_to) const;

	/// \brief Create a vector storage transaction instance
	VectorStorageTransaction createTransaction();
	
	/// \brief Get the list of concept class names defined
	/// \return the list
	ValueVariant conceptClassNames() const;

	/// \brief Get the list of indices of features represented by a learnt concept feature
	/// \param[in] conceptClass name identifying a class of concepts learnt
	/// \param[in] conceptid index (indices of learnt concepts starting from 1) 
	/// \return the resulting vector indices (index is order of insertion starting from 0)
	ValueVariant conceptFeatures( const std::string& conceptClass, int conceptid) const;

	/// \brief Get the number of concept features learned for a class
	/// \param[in] conceptClass name identifying a class of concepts learnt.
	/// \return the number of concept features and also the maximum number assigned to a feature (starting with 1)
	unsigned int nofConcepts( const std::string& conceptClass) const;

	/// \brief Get the set of learnt concepts of a class for a feature defined
	/// \param[in] conceptClass name identifying a class of concepts learnt
	/// \param[in] index index of vector in the order of insertion starting from 0
	/// \return the resulting concept feature indices (indices of learnt concepts starting from 1) (std::vector<int>)
	ValueVariant featureConcepts( const std::string& conceptClass, int index) const;

	/// \brief Get the vector assigned to a feature addressed by index
	/// \param[in] index index of the feature (starting from 0)
	/// return the vector (std::vector<double>)
	ValueVariant featureVector( int index) const;

	/// \brief Get the name of a feature by its index starting from 0
	/// \param[in] index index of the feature (starting from 0)
	/// \return the name of the feature defined 
	std::string featureName( int index) const;

	/// \brief Get the index starting from 0 of a feature by its name
	/// \param[in] name name of the feature
	/// \return index -1, if not found, else index of the feature to get the name of (index is order of insertion starting with 0)
	int featureIndex( const std::string& name) const;

	/// \brief Get the number of feature vectors defined
	/// \return the number of features
	unsigned int nofFeatures() const;

	/// \brief Get the configuration of this model
	/// \return the configuration string
	std::string config() const;

	/// \brief Controlled close to free resources (forcing free resources in interpreter context with garbage collector)
	void close();

private:
	friend class Context;
	VectorStorageClient( const HostObjectReference& objbuilder, const HostObjectReference& trace, const HostObjectReference& errorhnd_, const std::string& config);

	HostObjectReference m_errorhnd_impl;
	HostObjectReference m_trace_impl;
	HostObjectReference m_objbuilder_impl;
	HostObjectReference m_vector_storage_impl;
	std::string m_config;
};

class VectorStorageTransaction
{
public:
	/// \brief Copy constructor
	VectorStorageTransaction( const VectorStorageTransaction& o);

	/// \brief Destructor
	virtual ~VectorStorageTransaction(){}

	/// \brief Add named feature to vector storage
	/// \param[in] name unique name of the feature added
	/// \param[in] vec vector assigned to the feature
	void addFeature( 
			const std::string& name,
			const ValueVariant& vec);

	/// \brief Assign a concept (index) to a feature referenced by index
	/// \param[in] relationTypeName name of the relation
	/// \param[in] featidx index of the feature
	/// \param[in] conidx index of the concept
	void defineFeatureConceptRelation(
			const std::string& relationTypeName,
			int featidx,
			int conidx);
	/// \brief Commit of the transaction
	bool commit();

	/// \brief Rollback of the transaction
	void rollback();

	/// \brief Controlled close to free resources (forcing free resources in interpreter context with garbage collector)
	void close();

private:
	friend class VectorStorageClient;
	VectorStorageTransaction( const HostObjectReference& objbuilder, const HostObjectReference& storage, const HostObjectReference& trace, const HostObjectReference& errorhnd_, const std::string& config);

	HostObjectReference m_errorhnd_impl;
	HostObjectReference m_trace_impl;
	HostObjectReference m_objbuilder_impl;
	HostObjectReference m_vector_storage_impl;	
	HostObjectReference m_vector_transaction_impl;
};

/// \brief Forward declaration
class Query;

/// \brief Query evaluation program object representing an information retrieval scheme for documents in a storage.
class QueryEval
{
public:
	/// \brief Copy constructor
	QueryEval( const QueryEval& o);
	/// \brief Destructor
	virtual ~QueryEval(){}

	/// \brief Declare a term that is used in the query evaluation as structural element without beeing part of the query (for example punctuation used for match fields summarization)
	/// \param[in] set_ identifier of the term set that is used to address the terms
	/// \param[in] type_ feature type of the of the term
	/// \param[in] value_ feature value of the of the term
	void addTerm(
			const std::string& set_,
			const std::string& type_,
			const std::string& value_);

	/// \brief Declare a feature set to be used as selecting feature
	/// \param[in] set_ identifier of the term set addressing the terms to use for selection
	void addSelectionFeature( const std::string& set_);

	/// \brief Declare a feature set to be used as restriction
	/// \param[in] set_ identifier of the term set addressing the terms to use as restriction
	void addRestrictionFeature( const std::string& set_);

	/// \brief Declare a feature set to be used as exclusion
	/// \param[in] set_ identifier of the term set addressing the terms to use as exclusion
	void addExclusionFeature( const std::string& set_);

	/// \brief Declare a summarizer
	/// \param[in] name the name of the summarizer to add
	/// \param[in] config the configuration of the summarizer to add
	/// \param[in] debugAttributeName summarization attribute to use for debug info if requested
	void addSummarizer(
			const std::string& name,
			const ValueVariant& config,
			const std::string& debugAttributeName);


	/// \brief Add a weighting function to use as summand of the total document weight
	/// \param[in] name the name of the weighting function to add
	/// \param[in] config the configuration structure of the function to add
	/// \param[in] debugAttributeName summarization attribute to use for debug info if requested
	void addWeightingFunction(
			const std::string& name,
			const ValueVariant& config,
			const std::string& debugAttributeName);

	/// \brief Add a weighting formula to use for calculating the total weight from the weighting function results
	/// \param[in] defaultParameter default parameter values
	void addWeightingFormula(
			const std::string& source,
			const ValueVariant& defaultParameter);

	/// \brief Create a query to instantiate based on this query evaluation scheme
	/// \param[in] storage storage to execute the query on
	Query createQuery( const StorageClient& storage) const;

private:
	/// \brief Constructor used by Context
	friend class Context;
	QueryEval( const HostObjectReference& objbuilder, const HostObjectReference& trace, const HostObjectReference& errorhnd);

	friend class Query;
	HostObjectReference m_errorhnd_impl;
	HostObjectReference m_trace_impl;
	HostObjectReference m_objbuilder_impl;
	HostObjectReference m_queryeval_impl;
	const void* m_queryproc;
};

/// \brief Query program object representing a retrieval method for documents in a storage.
class Query
{
public:
	/// \brief Copy constructor
	Query( const Query& o);
	/// \brief Destructor
	virtual ~Query(){}

	/// \brief Create a feature from the query expression passed
	/// \param[in] set_ name of the feature set, this feature is addressed with
	/// \param[in] expr_ query expression that defines the postings of the feature and the variables attached
	/// \param[in] weight_ individual weight of the feature in the query
	/// \remark the query expression passed as parameter is refused if it does not contain exactly one element
	void defineFeature( 
			const std::string& set_,
			const ValueVariant& expr_,
			double weight_=1.0);

	/// \brief Define a posting iterator describing a document field addressable as feature
	/// \param[in] set_ name of the feature set, this feature is addressed with
	/// \param[in] metadataStart name of meta data element that defines the start of the document field, assumed as 1 if name is empty
	/// \param[in] metadataEnd name of meta data element that defines the end of the document field, assumed as max value if name is empty
	void defineDocFieldFeature(
			const std::string& set_,
			const std::string& metadataStart,
			const std::string& metadataEnd);

	/// \brief Define a meta data restriction
	/// \param[in] compareOp compare operator, one of "=","!=",">=","<=","<",">"
	/// \param[in] name of the meta data field (left side of comparison operator)
	/// \param[in] value numeric value to compare with the meta data field (right side of comparison operator)
	/// \param[in] newGroup true, if the restriction is not an alternative condition to the previous one defined (alternative conditions are evaluated as logical OR)
	void addMetaDataRestrictionCondition(
			const char* compareOp,
			const std::string& name,
			NumericVariant value,
			bool newGroup);

	/// \brief Define a meta data restriction
	/// \param[in] compareOp compare operator, one of "=","!=",">=","<=","<",">"
	/// \param[in] name of the meta data field (left side of comparison operator)
	/// \param[in] value numeric value to compare with the meta data field (right side of comparison operator)
	/// \param[in] newGroup true, if the restriction is not an alternative condition to the previous one defined (alternative conditions are evaluated as logical OR)
	void addMetaDataRestrictionCondition(
			const char* compareOp,
			const std::string& name,
			unsigned int value,
			bool newGroup);

	/// \brief Define a meta data restriction
	/// \param[in] compareOp compare operator, one of "=","!=",">=","<=","<",">"
	/// \param[in] name of the meta data field (left side of comparison operator)
	/// \param[in] value numeric value to compare with the meta data field (right side of comparison operator)
	/// \param[in] newGroup true, if the restriction is not an alternative condition to the previous one defined (alternative conditions are evaluated as logical OR)
	void addMetaDataRestrictionCondition(
			const char* compareOp,
			const std::string& name,
			int value,
			bool newGroup);

	/// \brief Define term statistics to use for a term for weighting it in this query
	/// \param[in] type_ query term type name
	/// \param[in] value_ query term value
	/// \param[in] stats_ the structure with the statistics to set (strus::TermStatistics)
	void defineTermStatistics(
			const std::string& type_,
			const std::string& value_,
			const ValueVariant& stats_);

	/// \brief Define the global statistics to use for weighting in this query
	/// \param[in] stats_ the structure with the statistics to set (strus::GlobalStatistics)
	void defineGlobalStatistics( const ValueVariant& stats_);

	/// \brief Define a set of documents the query is evaluated on. By default the query is evaluated on all documents in the storage
	/// \param[in] docnolist_ list of documents to evaluate the query on (std::vector<int>)
	void addDocumentEvaluationSet( const ValueVariant& docnolist_);

	/// \brief Set number of ranks to evaluate starting with the first rank (the maximum size of the result rank list)
	/// \param[in] maxNofRanks_ maximum number of results to return by this query
	void setMaxNofRanks( unsigned int maxNofRanks_);

	/// \brief Set the index of the first rank to be returned
	/// \param[in] minRank_ index of the first rank to be returned by this query
	void setMinRank( unsigned int minRank_);

	/// \brief Add a user for this query (as alternative role)
	/// \param[in] username_ an alternative name of a user for the evaluation of this query
	/// \note the user restriction applies if no user role specified in the query is allowed to see the document.
	void addUserName( const std::string& username_);

	/// \brief Assign values to variables of the weighting formula
	/// \param[in] parameter parameter values (std::map<std::string,double>)
	void setWeightingVariables( const ValueVariant& parameter);
	/// \brief Switch on debug mode that creates debug info of query evaluation methods and summarization as attributes of the query result
	/// \param[in] debug true if switched on, false if switched off (default off)
	/// \note Debug attributes are specified in the declaration of summarizers and weighting functions (3rd parameter of QueryEval::addSummarizer and QueryEval::addWeightingFunction)
	void setDebugMode( bool debug);

	/// \brief Evaluate this query and return the result
	/// \return the result (strus::QueryResult)
	ValueVariant evaluate() const;

	/// \brief Map the contents of the query to a readable string
	/// \return the string
	std::string tostring() const;

private:
	friend class QueryEval;
	Query( const HostObjectReference& objbuilder_impl_, const HostObjectReference& trace_impl_, const HostObjectReference& errorhnd_, const HostObjectReference& storage_impl_, const HostObjectReference& queryeval_impl_, const HostObjectReference& query_impl_, const void* queryproc_)
		:m_errorhnd_impl(errorhnd_),m_trace_impl(trace_impl_),m_objbuilder_impl(objbuilder_impl_),m_storage_impl(storage_impl_),m_queryeval_impl(queryeval_impl_),m_query_impl(query_impl_),m_queryproc(queryproc_)
	{}

	HostObjectReference m_errorhnd_impl;
	HostObjectReference m_trace_impl;
	HostObjectReference m_objbuilder_impl;
	HostObjectReference m_storage_impl;
	HostObjectReference m_queryeval_impl;
	HostObjectReference m_query_impl;
	const void* m_queryproc;
};

///\brief Implements browsing the documents of a storage without weighting query, just with a restriction on metadata
class DocumentBrowser
{
public:
	/// \brief Copy constructor
	DocumentBrowser( const DocumentBrowser& o);
	/// \brief Destructor
	~DocumentBrowser(){}

	/// \brief Define a meta data restriction condition on the documents visited
	/// \param[in] compareOp compare operator, one of "=","!=",">=","<=","<",">"
	/// \param[in] name of the meta data field (left side of comparison operator)
	/// \param[in] value numeric value to compare with the meta data field (right side of comparison operator)
	/// \param[in] newGroup true, if the restriction is not an alternative condition to the previous one defined (alternative conditions are evaluated as logical OR)
	/// \remark Metadata restrictions can only be defined before the first call of this DocumentBrowser::next()
	void addMetaDataRestrictionCondition(
			const char* compareOp, const std::string& name,
			double value, bool newGroup);

	/// \brief Define a meta data restriction condition on the documents visited
	/// \param[in] compareOp compare operator, one of "=","!=",">=","<=","<",">"
	/// \param[in] name of the meta data field (left side of comparison operator)
	/// \param[in] value numeric value to compare with the meta data field (right side of comparison operator)
	/// \param[in] newGroup true, if the restriction is not an alternative condition to the previous one defined (alternative conditions are evaluated as logical OR)
	/// \remark Metadata restrictions can only be defined before the first call of this DocumentBrowser::next()
	void addMetaDataRestrictionCondition(
			const char* compareOp, const std::string& name,
			unsigned int value, bool newGroup);

	/// \brief Define a meta data restriction condition on the documents visited
	/// \param[in] compareOp compare operator, one of "=","!=",">=","<=","<",">"
	/// \param[in] name of the meta data field (left side of comparison operator)
	/// \param[in] value numeric value to compare with the meta data field (right side of comparison operator)
	/// \param[in] newGroup true, if the restriction is not an alternative condition to the previous one defined (alternative conditions are evaluated as logical OR)
	/// \remark Metadata restrictions can only be defined before the first call of this DocumentBrowser::next()
	void addMetaDataRestrictionCondition(
			const char* compareOp, const std::string& name,
			int value, bool newGroup);

	///\brief Get the internal document number of the next document bigger or equal the document number passed
	///\param[in] docno_ document number to get the matching least upperbound from
	///\return the internal document number or 0 if no more documents defined
	int skipDoc( int docno_);

	///\brief Get an attribute of the current document visited
	///\return the internal document number or 0, if there is no one left
	std::string attribute( const std::string& name);

private:
	friend class StorageClient;
	DocumentBrowser(
		const HostObjectReference& objbuilder_impl_,
		const HostObjectReference& trace_impl_,
		const HostObjectReference& storage_impl_,
		const HostObjectReference& errorhnd_);

private:
	HostObjectReference m_errorhnd_impl;
	HostObjectReference m_trace_impl;
	HostObjectReference m_objbuilder_impl;
	HostObjectReference m_storage_impl;
	HostObjectReference m_restriction_impl;
	HostObjectReference m_postingitr_impl;
	HostObjectReference m_attributereader_impl;
	int m_docno;
};

/// \brief Object holding the global context of the strus information retrieval engine
/// \note There a two modes of this context object operating on a different base.
///	If you create this object without parameter, then the context is local.
///	In a local context you can load modules, define resources, etc. If you create
///	this object with a connection string as parameter, then all object created by
///	this context reside on the server (strusRpcServer) addressed with the connection string.
///	In this case loaded modules and resources are ignored. What modules to use is then
///	specified on server startup.
class Context
{
public:
	/// \brief Constructor for local mode with own module loader
	Context();
	/// \brief Constructor for local mode with own module loader
	/// \param[in] maxNofThreads the maximum number of threads used (for error handler context), 0 for default
	explicit Context( unsigned int maxNofThreads);
	/// \brief Constructor for local mode with own module loader and logging of all method call traces
	/// \param[in] maxNofThreads the maximum number of threads used (for error handler context), 0 for default
	/// \param[in] tracecfg configuration for generating method call traces
	explicit Context( unsigned int maxNofThreads, const std::string& tracecfg);
	/// \brief Constructor for remote mode (objects of the context are living on a server connected via RPC)
	/// \param[in] connectionstring RPC server connection string
	/// \warning The RPC mode is only desinged for trusted clients. It is highly insecure if not strictly used in a private network only.
	explicit Context( const std::string& connectionstring);
	/// \brief Constructor for remote mode (objects of the context are living on a server connected via RPC)
	/// \param[in] connectionstring RPC server connection string
	/// \param[in] maxNofThreads the maximum number of threads used (for error handler context), 0 for default
	/// \warning The RPC mode is only desinged for trusted clients. It is highly insecure if not strictly used in a private network only.
	Context( const std::string& connectionstring, unsigned int maxNofThreads);
	/// \brief Copy constructor
	Context( const Context& o);
	/// \brief Destructor
	~Context(){}

	/// \brief Check if there has an error occurred and throw if yes
	/// \remark Some bindings have coroutines and with a coroutine switch an error message might get lost, because error context is per thread. So if a coroutine switch is done without the last error fetched it might happen that the second coroutine gets the error of the first one. Call this function after calling some method without return value before a state where a context switch is possible.
	void checkErrors() const;

	/// \brief Load a module
	/// \param[in] name_ name of the module to load
	/// \remark Only implemented in local mode with own module loader (see constructors)
	void loadModule( const std::string& name_);

	/// \brief Add a path from where to try to load modules from
	/// \param[in] paths_ semicolon separated list of module search paths
	/// \remark Only implemented in local mode with own module loader (see constructors)
	void addModulePath( const std::string& paths_);

	/// \brief Define where to load analyzer resource files from
	/// \param[in] paths_ semicolon separated list of resource search paths
	/// \remark Only implemented in local mode with own module loader (see constructors)
	void addResourcePath( const std::string& paths_);

	/// \brief Create a statistics message processor instance
	/// \return the processor
	StatisticsProcessor createStatisticsProcessor( const std::string& name);

	/// \brief Create a storage client instance of the the default storage
	StorageClient createStorageClient();

	/// \brief Create a storage client instance
	/// \param[in] config_ configuration string of the storage client or empty, if the default remote storage of the RPC server is chosen,
	StorageClient createStorageClient( const std::string& config_);

	/// \brief Create a storage client instance of the the default remote storage of the RPC server
	VectorStorageClient createVectorStorageClient();

	/// \brief Create a vector storage client instance
	/// \param[in] config_ configuration string of the storage client or empty for the default storage
	VectorStorageClient createVectorStorageClient( const std::string& config_);

	/// \brief Create a new storage (physically) described by config
	/// \param[in] config_ storage configuration
 	/// \remark Fails if the storage already exists
	void createStorage( const std::string& config_);

	/// \brief Create a new storage (physically) described by config
	/// \param[in] config_ storage configuration
 	/// \remark Fails if the storage already exists
	void createVectorStorage( const std::string& config_);

	/// \brief Delete the storage (physically) described by config
	/// \note works also on vector storages
	/// \param[in] config_ storage description
	/// \note Handle this function carefully
	void destroyStorage( const std::string& config_);

	/// \brief Detect the type of document from its content
	/// \param[in] content the document content to classify
	/// \return the document class (analyzer::DocumentClass)
	ValueVariant detectDocumentClass( const std::string& content);

	/// \brief Create a document analyzer instance
	/// \param[in] segmentername_ name of the segmenter to use (if empty then the default segmenter is used)
	DocumentAnalyzer createDocumentAnalyzer( const std::string& segmentername_="");

	/// \brief Create a query analyzer instance
	QueryAnalyzer createQueryAnalyzer();

	/// \brief Create a query evaluation instance
	QueryEval createQueryEval();

	/// \brief Force cleanup to circumvent object pooling mechanisms in an interpreter context
	void close();

private:
	void initStorageObjBuilder();
	void initAnalyzerObjBuilder();

private:
	HostObjectReference m_errorhnd_impl;
	HostObjectReference m_moduleloader_impl;
	HostObjectReference m_rpc_impl;
	HostObjectReference m_trace_impl;
	HostObjectReference m_storage_objbuilder_impl;
	HostObjectReference m_analyzer_objbuilder_impl;
	const TextProcessorInterface* m_textproc;
};

}}//namespace
#endif


