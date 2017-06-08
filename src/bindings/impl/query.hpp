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
#include "strus/numericVariant.hpp"
#include "strus/queryResult.hpp"
#include "impl/objectref.hpp"
#include <vector>
#include <string>

namespace strus {
namespace bindings {

typedef papuga_ValueVariant ValueVariant;

///\brief Forward declaration
class QueryImpl;
///\brief Forward declaration
class StorageClientImpl;

/// \brief Query evaluation program object representing an information retrieval scheme for documents in a storage.
class QueryEvalImpl
{
public:
	/// \brief Destructor
	virtual ~QueryEvalImpl(){}

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
	/// \param[in] parameter the parameters of the summarizer to add (parameter name 'debug' reserved for declaring the debug info attribute)
	/// \param[in] resultnames the mapping of result names
	void addSummarizer(
			const std::string& name,
			const ValueVariant& parameter,
			const ValueVariant& resultnames=ValueVariant());

	/// \brief Add a weighting function to use as summand of the total document weight
	/// \param[in] name the name of the weighting function to add
	/// \param[in] parameter the parameters of the weighting function to add
	void addWeightingFunction(
			const std::string& name,
			const ValueVariant& parameter);

	/// \brief Add a weighting formula to use for calculating the total weight from the weighting function results
	/// \param[in] defaultParameter default parameter values
	void addWeightingFormula(
			const std::string& source,
			const ValueVariant& defaultParameter);

	/// \brief Create a query to instantiate based on this query evaluation scheme
	/// \param[in] storage storage to execute the query on
	QueryImpl* createQuery( StorageClientImpl* storage) const;

private:
	/// \brief Constructor used by Context
	friend class ContextImpl;
	QueryEvalImpl( const ObjectRef& objbuilder, const ObjectRef& trace, const ObjectRef& errorhnd);

	friend class QueryImpl;
	mutable ObjectRef m_errorhnd_impl;
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_queryeval_impl;
	const QueryProcessorInterface* m_queryproc;
};


/// \brief Query program object representing a retrieval method for documents in a storage.
class QueryImpl
{
public:
	/// \brief Destructor
	virtual ~QueryImpl(){}

	/// \brief Create a feature from the query expression passed
	/// \param[in] set_ name of the feature set, this feature is addressed with
	/// \param[in] expr_ query expression that defines the postings of the feature and the variables attached
	/// \param[in] weight_ individual weight of the feature in the query
	/// \remark the query expression passed as parameter is refused if it does not contain exactly one element
	void defineFeature( 
			const std::string& set_,
			const ValueVariant& expr_,
			double weight_=1);

	/// \brief Define a meta data restriction
	/// \param[in] expression meta data expression tree interpreted as CNF (conjunctive normalform "AND" of "OR"s)
	///	leafs of the tree are 3-tuples of the form {operator,name,operand} with
	///		operator: one of "=","!=",">=","<=","<",">"
	///		name: name of meta data element
	///		value: numeric value to compare with the meta data field (right side of comparison operator)
	///	if the tree has depth 1 (single node), then it is interpreted as single condition
	///	if the tree has depth 2 (list of nodes), then it is interpreted as intersection "AND" of its leafs
	///	an "OR" of conditions without "AND" is therefore expressed as list of list of structures, e.g. '[[["<=","date","1.1.1970"], [">","weight",1.0]]]' <=> 'date <= "1.1.1970" OR weight > 1.0' and '[["<=","date","1.1.1970"], [">","weight",1.0]]' <=> 'date <= "1.1.1970" AND weight > 1.0'
	void addMetaDataRestriction( const ValueVariant& expression);

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
	QueryResult* evaluate() const;

	/// \brief Map the contents of the query to a readable string
	/// \return the string
	std::string tostring() const;

private:
	friend class QueryEvalImpl;
	QueryImpl( const ObjectRef& objbuilder_impl_, const ObjectRef& trace_impl_, const ObjectRef& errorhnd_, const ObjectRef& storage_impl_, const ObjectRef& queryeval_impl_, const ObjectRef& query_impl_, const QueryProcessorInterface* queryproc_)
		:m_errorhnd_impl(errorhnd_),m_trace_impl(trace_impl_),m_objbuilder_impl(objbuilder_impl_),m_storage_impl(storage_impl_),m_queryeval_impl(queryeval_impl_),m_query_impl(query_impl_),m_queryproc(queryproc_)
	{}

	mutable ObjectRef m_errorhnd_impl;
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_storage_impl;
	ObjectRef m_queryeval_impl;
	ObjectRef m_query_impl;
	const QueryProcessorInterface* m_queryproc;
};

}}//namespace
#endif

