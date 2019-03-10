/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/sentence.hpp"
#include "strus/lib/storage_objbuild.hpp"
#include "strus/sentenceAnalyzerInstanceInterface.hpp"
#include "strus/sentenceLexerInstanceInterface.hpp"
#include "strus/sentenceLexerContextInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/base/configParser.hpp"
#include "private/internationalization.hpp"
#include "serializer.hpp"
#include "deserializer.hpp"
#include "callResultUtils.hpp"

using namespace strus;
using namespace strus::bindings;

SentenceAnalyzerImpl::~SentenceAnalyzerImpl()
{}

SentenceAnalyzerImpl::SentenceAnalyzerImpl( const ObjectRef& trace_impl, const ObjectRef& objbuilder_impl, const ObjectRef& analyzer_impl, const ObjectRef& lexer_impl, const ObjectRef& errorhnd_, const ValueVariant& config)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl( trace_impl)
	,m_objbuilder_impl( objbuilder_impl)
	,m_analyzer_impl( analyzer_impl)
	,m_lexer_impl( lexer_impl)
{
	SentenceAnalyzerInstanceInterface* analyzer = m_analyzer_impl.getObject<SentenceAnalyzerInstanceInterface>();
	if (!analyzer) throw strus::runtime_error( _TXT("calling sentence analyzer method after close"));
	SentenceLexerInstanceInterface* lexer = m_lexer_impl.getObject<SentenceLexerInstanceInterface>();
	if (!lexer) throw strus::runtime_error( _TXT("calling sentence analyzer method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	Deserializer::buildSentenceAnalyzer( analyzer, lexer, config, errorhnd);
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
}

Struct SentenceAnalyzerImpl::analyze( const std::string& source, int maxNofResults, double minWeight) const
{
	const SentenceAnalyzerInstanceInterface* analyzer = m_analyzer_impl.getObject<SentenceAnalyzerInstanceInterface>();
	const SentenceLexerInstanceInterface* lexer = m_lexer_impl.getObject<SentenceLexerInstanceInterface>();

	std::vector<SentenceGuess> res = analyzer->analyzeSentence( lexer, source, maxNofResults, minWeight);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
	Struct rt;
	strus::bindings::Serializer::serialize( &rt.serialization, res, true/*deep*/);
	rt.release();
	return rt;
}



