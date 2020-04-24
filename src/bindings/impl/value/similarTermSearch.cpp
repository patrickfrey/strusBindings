/*
 * Copyright (c) 2020 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Term expression as result of a query sentence analysis
#include "impl/value/similarTermSearch.hpp"

using namespace strus;
using namespace strus::bindings;

SimilarTermSearch::SimilarTermSearch( const std::string& simtype, const strus::SentenceLexerInstanceInterface* lexer, std::vector<strus::WeightedSentenceTerm>& termlist, double minSimilarity, int maxNofResults, double minNormalizedWeight)
{
	m_ar = lexer->similarTerms( simtype, termlist, minSimilarity, maxNofResults, minNormalizedWeight);
}


