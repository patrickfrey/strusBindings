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
	/// \param[in] source source to analyzer
	/// \example "The best football manager in the world"
	/// \example "Discography of the rolling stones"
	/// \param[in] maxNofResults maximum number of results to return
	/// \example 2
	/// \example 5
	/// \return the list of the most probable query term lists with weights
	Struct analyze( const std::string& source, unsigned int maxNofResults) const;

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

