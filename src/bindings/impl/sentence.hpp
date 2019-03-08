/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_IMPL_SENTENCE_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_SENTENCE_HPP_INCLUDED
#include "papuga/valueVariant.h"
#include "impl/value/objectref.hpp"
#include "impl/value/struct.hpp"
#include "impl/value/termExpression.hpp"
#include <vector>
#include <string>

namespace strus {
namespace bindings {

typedef papuga_ValueVariant ValueVariant;

/// \class SentenceAnalyzerImpl
/// \brief Object representing a sentence analyzer for the query
/// \note Currently the only way to construct a sentence analyzer instance is to call VectorStorageClient::createSentenceAnalyzer( config)
class SentenceAnalyzerImpl
{
public:
	/// \brief Destructor
	virtual ~SentenceAnalyzerImpl();

	/// \brief Split a sentence into lexems and find the most probable configurations of them and get a weighted ranked list of the best candidates
	/// \param[in] source source to analyze
	/// \example "best football manager in the world"
	/// \example "discography of the rolling stones"
	/// \param[in] maxNofResults maximum number of results to return
	/// \example 2
	/// \example 5
	/// \return the list of the most probable query term lists with weights
	/// \example [ sentence: "norm", weight: 1.0, terms: [[type: "M", value: "best"],[type: "N", value: "football_manager"],[type: "T", value: "in"],[type: "T", value: "the"],[type: "N", value: "world"]] ]
	Struct analyze( const std::string& source, unsigned int maxNofResults) const;

	/// \brief Analyze all terms of a selected type as sentences and duplicate the term expression for each instance found
	/// \param[in] termExpression expression tree to analyze
	/// \example "best football manager in the world"
	/// \example "discography of the rolling stones"
	/// \param[in] sentenceFeatType type of the feature containing sentences to analyze
	/// \example "SENT"
	/// \param[in] maxNofResults maximum number of results to return
	/// \example 4
	/// \param[in] minWeight minimum weight to accept of a result, best result has weight 1.0 and the others are normalized to be smaller or equal to 1.0
	/// \example 0.99
	/// \example 0.8
	/// \return the term expression tree after analysis
	/// \example [ sentence: "norm", weight: 1.0, terms: [[type: "M", value: "best"],[type: "N", value: "football_manager"],[type: "T", value: "in"],[type: "T", value: "the"],[type: "N", value: "world"]] ]
	TermExpression* analyzeTermExpression( const ValueVariant& termExpression, const std::string& sentenceFeatType, int maxNofResults, double minWeight) const;

	/// \brief Controlled close to free resources (forcing free resources in interpreter context with garbage collector)
	/// \remark This method is not implicitely called with the destructor because it might be a complicated operation that cannot be afforded in panic shutdown.
	/// \note the method does not have to be called necessarily.
	void close();

private:
	friend class VectorStorageClientImpl;
	SentenceAnalyzerImpl( const ObjectRef& trace, const ObjectRef& objbuilder, const ObjectRef& analyzer, const ObjectRef& lexer, const ObjectRef& errorhnd_, const ValueVariant& config);

	mutable ObjectRef m_errorhnd_impl;
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_analyzer_impl;
	ObjectRef m_lexer_impl;
};

}}//namespace
#endif

