/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/sentence.hpp"
#include "strus/lib/storage_objbuild.hpp"
#include "strus/sentenceLexerInstanceInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/base/configParser.hpp"
#include "private/internationalization.hpp"
#include "serializer.hpp"
#include "deserializer.hpp"
#include "callResultUtils.hpp"

using namespace strus;
using namespace strus::bindings;

SentenceLexerImpl::~SentenceLexerImpl()
{}

SentenceLexerImpl::SentenceLexerImpl( const ObjectRef& trace_impl, const ObjectRef& objbuilder_impl, const ObjectRef& lexer_impl, const ObjectRef& errorhnd_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl( trace_impl)
	,m_objbuilder_impl( objbuilder_impl)
	,m_lexer_impl( lexer_impl)
{}

Struct SentenceLexerImpl::call( const std::string& source, int maxNofResults, double minWeight) const
{
	const SentenceLexerInstanceInterface* lexer = m_lexer_impl.getObject<SentenceLexerInstanceInterface>();

	std::vector<SentenceGuess> res = lexer->call( source, maxNofResults, minWeight);
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



