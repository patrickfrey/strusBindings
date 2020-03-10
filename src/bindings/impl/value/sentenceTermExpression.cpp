/*
 * Copyright (c) 2020 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Term expression as result of a query sentence analysis
#include "sentenceTermExpression.hpp"
#include "private/internationalization.hpp"

using namespace strus;
using namespace strus::bindings;

SentenceTermExpression::SentenceTermExpression(
		const QueryFeatureExpansionMap& expansionMap,
		const strus::SentenceLexerInstanceInterface* lexer,
		std::vector<strus::SentenceGuess>& guess)
{
	std::vector<strus::SentenceGuess>::iterator gi = guess.begin(), ge = guess.end();
	for (int gidx=0; gi != ge; ++gi,++gidx)
	{
		if (gidx == 0)
		{
			std::vector<strus::SentenceTerm>::const_iterator ti = gi->terms().begin(), te = gi->terms().end();
			for (; ti != te; ++ti)
			{
				m_ar.push_back( strus::SentenceTermList());
				m_ar.back().push_back( *ti);
			}
		}
		else if (gi->terms().size() == m_ar.size())
		{
			std::vector<strus::SentenceTerm>::const_iterator ti = gi->terms().begin(), te = gi->terms().end();
			std::vector<strus::SentenceTermList>::iterator ai = m_ar.begin(), ae = m_ar.end();
			for (; ti != te; ++ti,++ai)
			{
				std::vector<strus::SentenceTerm>::const_iterator ei = ai->begin(), ee = ai->end();
				for (;ei != ee; ++ei)
				{
					if (ei->value() == ti->value())
					{
						if (ei->type() != ti->type())
						{
							ai->push_back( *ti);
						}
						break;
					}
				}
				if (ei == ee)
				{
					guess.erase( guess.begin() + gidx);
					gidx--;
					gi = guess.begin() + gidx;
					ge = guess.end();
					break;
				}
			}
		}
	}
	// Query expansion step:
	std::size_t ai = 0, ae = m_ar.size();
	for (; ai != ae; ++ai)
	{
		std::size_t ei = 0, ee = m_ar[ai].size();
		for (;ei != ee; ++ei)
		{
			const strus::SentenceTerm& term = m_ar[ ai][ ei];
			QueryFeatureExpansionMap::const_iterator xi = expansionMap.find( term.type());
			if (xi != expansionMap.end())
			{
				std::vector<SentenceTerm> termlist;
				termlist.push_back( term);
				std::vector<SentenceTerm> simterms =
					lexer->similarTerms( term.type(), termlist,
							xi->second.similarity, xi->second.maxNofResults,
							xi->second.minNormalizedWeight);
				std::vector<SentenceTerm>::const_iterator si = simterms.begin(), se = simterms.end();
				for (; si != se; ++si)
				{
					if (*si != term)
					{
						m_ar[ ai].push_back( *si);
					}
				}
			}
		}
	}
}

