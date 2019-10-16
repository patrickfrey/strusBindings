/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_IMPL_QUERY_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_QUERY_HPP_INCLUDED
#include "papuga/valueVariant.h"
#include "strus/queryProcessorInterface.hpp"
#include "strus/queryInterface.hpp"
#include "strus/numericVariant.hpp"
#include "strus/queryResult.hpp"
#include "impl/value/objectref.hpp"
#include "impl/value/struct.hpp"
#include <vector>
#include <string>

namespace strus {
namespace bindings {

typedef papuga_ValueVariant ValueVariant;

///\brief Forward declaration
class QueryImpl;
///\brief Forward declaration
class StorageClientImpl;

/// \class QueryEvalImpl
/// \brief Query evaluation program object representing an information retrieval scheme for documents in a storage.
/// \note The only way to construct a query eval instance is to call Context::createQueryEval()
class QueryEvalImpl
{
public:
	/// \brief Destructor
	virtual ~QueryEvalImpl(){}

	/// \brief Declare a term that is used in the query evaluation as structural element without beeing part of the query (for example punctuation used for match fields summarization)
	/// \param[in] set identifier of the term set that is used to address the terms
	/// \example "eos"
	/// \param[in] type feature type of the of the term
	/// \example "punct"
	/// \param[in] value feature value of the of the term
	/// \example "."
	void addTerm(
			const std::string& set,
			const std::string& type,
			const std::string& value);

	/// \brief Declare a feature set to be used as selecting feature
	/// \param[in] set identifier of the term set addressing the terms to use for selection
	/// \example "select"
	void addSelectionFeature( const std::string& set);

	/// \brief Declare a feature set to be used as restriction
	/// \param[in] set identifier of the term set addressing the terms to use as restriction
	/// \example "restrict"
	void addRestrictionFeature( const std::string& set);

	/// \brief Declare a feature set to be used as exclusion
	/// \param[in] set identifier of the term set addressing the terms to use as exclusion
	/// \example "exclusion"
	void addExclusionFeature( const std::string& set);

	/// \brief Declare a summarizer
	/// \example addSummarizer( "attribute" [["name" "docid"] ["debug" "debug_attribute"]] )
	/// \example addSummarizer( "metadata"  [["name" "cross"] ["debug" "debug_metadata" ]] )
	/// \param[in] name the name of the summarizer to add
	/// \example "matchphrase"
	/// \example "matchpos"
	/// \example "attribute"
	/// \param[in] parameter the parameters of the summarizer to add (parameter name 'debug' reserved for declaring the debug info attribute)
	/// \example [ sentencesize: 40 windowsize: 100 cardinality: 5 ]
	/// \param[in] featuresets the mapping of used feature sets, list of key value pairs assigning feature roles to feature sets, key "role", value(s) "set" (optional)
	/// \example [ "weight", "word" ]
	/// \example [ ["struct", "punct"], [ "weight", "word" ] ]
	/// \example [ [ role: "weight", set: "word" ] ]
	/// \example [ [ role: "weight", set: "word" ], [ role: "struct", set: "punct"]]
	/// \param[in] resultnames the mapping of result names (optional)
	void addSummarizer(
			const std::string& name,
			const ValueVariant& parameter,
			const ValueVariant& featuresets=ValueVariant(),
			const ValueVariant& resultnames=ValueVariant());

	/// \brief Add a weighting function to use as summand of the total document weight
	/// \example addWeightingFunction( "tf", [match:[feature:"seek"] debug:"debug_weighting"] )
	/// \param[in] name the name of the weighting function to add
	/// \example "BM25"
	/// \param[in] parameter the parameters of the weighting function to add
	/// \example [ b:0.75 k:1.2 avgdoclen:1000 match:[feature:"seek"] debug:"debug_weighting" ]
	/// \param[in] featuresets the mapping of used feature sets, list of key value pairs assigning feature roles to feature sets, key "role", value(s) "set" (optional)
	/// \example [ "weight", "word" ]
	/// \example [ ["struct", "punct"], [ "weight", "word" ] ]
	/// \example [ [ role: "weight", set: "word" ] ]
	/// \example [ [ role: "weight", set: "word" ], [ role: "struct", set: "punct"]]
	void addWeightingFunction(
			const std::string& name,
			const ValueVariant& parameter,
			const ValueVariant& featuresets=ValueVariant());

	/// \brief Define the weighting formula to use for calculating the total weight from the weighting function results (sum of the weighting function results is the default)
	/// \example defineWeightingFormula( "_0 / _1" )
 	/// \param[in] source of the weighting formula
	/// \example "_0 / ln( _1 + 1)"
	/// \param[in] defaultParameter default parameter values
	void defineWeightingFormula(
			const std::string& source,
			const ValueVariant& defaultParameter=ValueVariant());

	/// \brief Create a query to instantiate based on this query evaluation scheme
	/// \param[in] storage storage to execute the query on
	/// \return the query instance
	QueryImpl* createQuery( StorageClientImpl* storage) const;

	/// \brief Introspect a structure starting from a root path
	/// \param[in] path list of idenfifiers describing the access path to the element to introspect
	/// \return the structure to introspect starting from the path
	Struct introspection( const ValueVariant& path) const;

private:
	/// \brief Constructor used by Context
	friend class ContextImpl;
	QueryEvalImpl( const ObjectRef& trace, const ObjectRef& objbuilder, const ObjectRef& errorhnd);

	friend class QueryImpl;
	mutable ObjectRef m_errorhnd_impl;
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_queryeval_impl;
	const QueryProcessorInterface* m_queryproc;
};


/// \class QueryImpl
/// \brief Query program object representing a retrieval method for documents in a storage.
/// \note The only way to construct a query instance is to call QueryEval::createQuery( storage)
class QueryImpl
{
public:
	/// \brief Destructor
	virtual ~QueryImpl(){}

	/// \brief Create a feature from the query expression passed
	/// \example addFeature( "select" [ "contains" 0 1 ["word" "hello"] ["word" "world"] ] )
	/// \example addFeature( "titlefield" [ from:"title_start" to:"title_end"] )
	/// \param[in] set name of the feature set, this feature is addressed with
	/// \example "select"
	/// \example "seek"
	/// \param[in] expr query expression that defines the postings of the feature and the variables attached
	/// \example [ "contains" 0 1 ["word" "hello"] ["word" "world"] ]
	/// \example [ from:"title_start" to:"title_end"]
	/// \remark The query expression passed as parameter is refused if it does not contain exactly one element
	/// \param[in] weight individual weight of the feature in the query
	/// \example 0.75
	/// \example 1.0
	/// \example 2.5
	void addFeature( 
			const std::string& set,
			const ValueVariant& expr,
			double weight=1);

	/// \brief Define a meta data restriction
	/// \param[in] expression meta data expression tree interpreted as CNF (conjunctive normalform "AND" of "OR"s)
	/// \note leafs of the expression tree are 3-tuples of the form {operator,name,operand} with
	///		operator: one of "=","!=",">=","<=","<",">"
	///		name: name of meta data element
	///		value: numeric value to compare with the meta data field (right side of comparison operator)
	///	if the tree has depth 1 (single node), then it is interpreted as single condition
	///	if the tree has depth 2 (list of nodes), then it is interpreted as intersection "AND" of its leafs
	///	an "OR" of conditions without "AND" is therefore expressed as list of list of structures, e.g. '[[["<=","date","1.1.1970"], [">","weight",1.0]]]' <=> 'date <= "1.1.1970" OR weight > 1.0' and '[["<=","date","1.1.1970"], [">","weight",1.0]]' <=> 'date <= "1.1.1970" AND weight > 1.0'
	/// \example  [ [["=" "country" 12] ["=" "country" 17]]  ["<" "year" "2007"] ]
	/// \example  ["<" "year" "2002"]
	void addMetaDataRestriction( const ValueVariant& expression);

	/// \brief Define term statistics to use for a term for weighting it in this query
	/// \example defineTermStatistics( "word" "game" [ df: 74653 ] )
	/// \param[in] type query term type name
	/// \example "word"
	/// \param[in] value query term value
	/// \example "game"
	/// \param[in] stats the structure with the statistics to set or the number of documents as single atomic value
	/// \example [ df: 74653 ]
	/// \example 74653
	void defineTermStatistics(
			const std::string& type,
			const std::string& value,
			const ValueVariant& stats);

	/// \brief Define the global statistics to use for weighting in this query
	/// \param[in] stats the structure with the statistics to set
	/// \example [ nofdocs: 1234331 ]
	void defineGlobalStatistics( const ValueVariant& stats);

	/// \brief Define a set of documents the query is evaluated on. By default the query is evaluated on all documents in the storage
	/// \param[in] docnolist list of documents to evaluate the query on (array of positive integers)
	/// \example [1,23,2345,3565,4676,6456,8855,12203]
	void addDocumentEvaluationSet( const ValueVariant& docnolist);

	/// \brief Set number of ranks to evaluate starting with the first rank (the maximum size of the result rank list)
	/// \param[in] maxNofRanks maximum number of results to return by this query
	/// \example 20
	/// \example 50
	/// \example 5
	void setMaxNofRanks( int maxNofRanks);

	/// \brief Set the index of the first rank to be returned
	/// \param[in] minRank index, starting with 0, of the first rank to be returned by this query
	/// \example 0
	/// \example 10
	/// \example 20
	void setMinRank( int minRank);

	/// \brief Flag that marks the result of this query to be used as input of a merge of multiple query results ('QueryResult::merge')
	/// \note In this case minRank and maxNofRanks have to be set to 0 and minRank+maxNofRanks for merge to work correctly
	/// \note This method is intented for the mappings in the webrequest context, because there we have no possibility for control structures and calculations
	/// \note default is false
	/// \param[in] yes true if the result of this query is used as input of a merge of multiple query results, false else
	void useMergeResult( bool yes=true);

	/// \brief Allow read access to documents having a specific ACL tag
	/// \note If no ACL tags are specified, then all documents are potential candidates for the result
	/// \param[in] rolelist Add ACL tag or list of ACL tags that selects documents to be candidates of the result
	/// \example ["public" "devel" "sys"]
	void addAccess( const ValueVariant& rolelist);

	/// \brief Assign values to variables of the weighting formula
	/// \param[in] parameter parameter values (map of variable names to floats)
	void setWeightingVariables( const ValueVariant& parameter);

	/// \brief Switch on debug mode that creates debug info of query evaluation methods and summarization as attributes of the query result
	/// \note Debug attributes are specified in the declaration of summarizers and weighting functions (3rd parameter of QueryEval::addSummarizer and QueryEval::addWeightingFunction)
	/// \param[in] debug true if switched on, false if switched off (default off)
	void setDebugMode( bool debug);

	/// \brief Evaluate this query and return the result
	/// \return the result (strus::QueryResult)
	QueryResult* evaluate() const;

	/// \brief Introspect a structure starting from a root path
	/// \param[in] path list of idenfifiers describing the access path to the element to introspect
	/// \return the structure to introspect starting from the path
	Struct introspection( const ValueVariant& path=ValueVariant()) const;

private:
	friend class QueryEvalImpl;
	QueryImpl( const ObjectRef& trace_impl_, const ObjectRef& objbuilder_impl_, const ObjectRef& errorhnd_, const ObjectRef& storage_impl_, const ObjectRef& queryeval_impl_, const ObjectRef& query_impl_, const QueryProcessorInterface* queryproc_)
		:m_errorhnd_impl(errorhnd_),m_trace_impl(trace_impl_),m_objbuilder_impl(objbuilder_impl_),m_storage_impl(storage_impl_),m_queryeval_impl(queryeval_impl_),m_query_impl(query_impl_),m_queryproc(queryproc_),m_useMergeResult(false),m_minRank(0),m_maxNofRanks(QueryInterface::DefaultMaxNofRanks)
	{}

	mutable ObjectRef m_errorhnd_impl;
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_storage_impl;
	ObjectRef m_queryeval_impl;
	ObjectRef m_query_impl;
	const QueryProcessorInterface* m_queryproc;
	bool m_useMergeResult;
	int m_minRank;
	int m_maxNofRanks;
};

/// \class QueryResultMergerImpl
/// \brief Object used to merge ranklists in the case of a distributed query evaluation.
/// \remark You might use the method ContextImpl::mergeQueryResults for this if it's easier (e.g. in scripting languages)
/// \note This object is intended for request handling where we have only mappings and no control structures as in scripting languages
class QueryResultMergerImpl
{
public:
	/// \brief Destructor
	virtual ~QueryResultMergerImpl(){}

	/// \brief Set number of ranks to evaluate starting with the first rank (the maximum size of the result rank list)
	/// \param[in] maxNofRanks maximum number of results to return by this query
	/// \example 20
	/// \example 50
	/// \example 5
	void setMaxNofRanks( int maxNofRanks);

	/// \brief Set the index of the first rank to be returned
	/// \param[in] minRank index, starting with 0, of the first rank to be returned by this query
	/// \example 0
	/// \example 10
	/// \example 20
	void setMinRank( int minRank);

	/// \brief Take a list of query results as input and return a merged query result
	/// \param[in] res one query result or a list of query results to merge
	void addQueryResult( const ValueVariant& res);

	/// \brief Flag that marks the result of this query to be used as input of a merge of multiple query results ('QueryResult::merge')
	/// \note In this case minRank and maxNofRanks have to be set to 0 and minRank+maxNofRanks for merge to work correctly
	/// \note This method is intented for the mappings in the webrequest context, because there we have no possibility for control structures and calculations
	/// \note default is false
	/// \param[in] yes true if the result of this query is used as input of a merge of multiple query results, false else
	void useMergeResult( bool yes=true);

	/// \brief Merge all added query results to one result
	/// \return the result (strus::QueryResult)
	QueryResult* evaluate() const;

	/// \brief Introspect a structure starting from a root path
	/// \param[in] path list of idenfifiers describing the access path to the element to introspect
	/// \return the structure to introspect starting from the path
	Struct introspection( const ValueVariant& path=ValueVariant()) const;

private:
	friend class ContextImpl;
	QueryResultMergerImpl( const ObjectRef& trace_impl_, const ObjectRef& errorhnd_)
		:m_errorhnd_impl(errorhnd_),m_trace_impl(trace_impl_),m_useMergeResult(false),m_minRank(0),m_maxNofRanks(QueryInterface::DefaultMaxNofRanks),m_ar()
	{}

	mutable ObjectRef m_errorhnd_impl;
	ObjectRef m_trace_impl;
	bool m_useMergeResult;
	int m_minRank;
	int m_maxNofRanks;
	std::vector<QueryResult> m_ar;
};

}}//namespace
#endif

