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

/// \class SentenceLexerImpl
/// \brief Object representing a sentence analyzer for the query
/// \note Currently the only way to construct a sentence analyzer instance is to call VectorStorageClient::createSentenceLexer( config)
class SentenceLexerImpl
{
public:
	/// \brief Destructor
	virtual ~SentenceLexerImpl();

	/// \brief Split a sentence into lexems and find the most probable configurations of them and get a weighted ranked list of the best candidates
	/// \param[in] source source to analyze
	/// \example "best football manager in the world"
	/// \example "discography of the rolling stones"
	/// \param[in] maxNofResults maximum number of results to return or -1 if not specified
	/// \example 2
	/// \example 5
	/// \example -1
	/// \param[in] minWeight defines cut of results with a lower weight
	/// \example 0.8
	/// \example 0.9
	/// \return the list of the most probable query term lists with weights
	/// \example [ sentence: weight: 1.0, terms: [[type: "M", value: "best"],[type: "N", value: "football_manager"],[type: "T", value: "in"],[type: "T", value: "the"],[type: "N", value: "world"]] ]
	Struct call( const std::string& source, int maxNofResults, double minWeight) const;

private:
	friend class VectorStorageClientImpl;
	SentenceLexerImpl( const ObjectRef& trace, const ObjectRef& objbuilder, const ObjectRef& lexer, const ObjectRef& errorhnd_);

	friend class QueryAnalyzerImpl;
	mutable ObjectRef m_errorhnd_impl;
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_lexer_impl;
};

}}//namespace
#endif

