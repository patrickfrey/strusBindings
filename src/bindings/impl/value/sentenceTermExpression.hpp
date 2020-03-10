/*
 * Copyright (c) 2020 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_IMPL_SENTENCE_TERM_EXPRESSION_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_SENTENCE_TERM_EXPRESSION_HPP_INCLUDED
/// \brief Term expression as result of a query sentence analysis
#include "strus/storage/sentenceTerm.hpp"
#include "strus/storage/sentenceGuess.hpp"
#include "strus/sentenceLexerInstanceInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "structDefs.hpp"
#include <vector>
#include <string>

namespace strus {
namespace bindings {

/// \brief Analyzed sentence term expression structure
class SentenceTermExpression
{
public:
	SentenceTermExpression( const QueryFeatureExpansionMap& expansionMap, const strus::SentenceLexerInstanceInterface* lexer, std::vector<strus::SentenceGuess>& guess);

	typedef std::vector<strus::SentenceTermList>::const_iterator const_iterator;

	const_iterator begin() const		{return m_ar.begin();}
	const_iterator end() const		{return m_ar.end();}
private:
	std::vector<strus::SentenceTermList> m_ar;
};

}}//namespace
#endif

