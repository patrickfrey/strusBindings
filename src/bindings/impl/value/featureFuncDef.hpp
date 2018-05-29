/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDINGS_IMPL_VALUE_FEATURE_FUNCTION_DEF_HPP_INCLUDED
#define _STRUS_BINDINGS_IMPL_VALUE_FEATURE_FUNCTION_DEF_HPP_INCLUDED
/// \brief Helper class to parse normalizers and tokenizers
/// \file featureFuncDef.hpp
#include "strus/base/shared_ptr.hpp"
#include "strus/analyzerObjectBuilderInterface.hpp"
#include "deserializer.hpp"
#include <stdexcept>

namespace strus {
namespace bindings {

struct FeatureFuncDef
{
	std::vector<Reference<NormalizerFunctionInstanceInterface> > normalizers_ref;
	std::vector<NormalizerFunctionInstanceInterface*> normalizers;
	Reference<TokenizerFunctionInstanceInterface> tokenizer;

	FeatureFuncDef( const TextProcessorInterface* textproc,
			const ValueVariant& tokenizer_,
			const ValueVariant& normalizers_,
			ErrorBufferInterface* errorhnd)
	{
		if (papuga_ValueVariant_defined( &tokenizer_))
		{
			tokenizer = Deserializer::getTokenizer( tokenizer_, textproc, errorhnd);
		}
		normalizers_ref = Deserializer::getNormalizers( normalizers_, textproc, errorhnd);
		std::vector<Reference<NormalizerFunctionInstanceInterface> >::iterator ni = normalizers_ref.begin(), ne = normalizers_ref.end();
		for (; ni != ne; ++ni)
		{
			normalizers.push_back( ni->get());
		}
	}

	void release()
	{
		(void)tokenizer.release();
		std::vector<Reference<NormalizerFunctionInstanceInterface> >::iterator
			ni = normalizers_ref.begin(), ne = normalizers_ref.end();
		for (; ni != ne; ++ni) (void)ni->release();
	}
};

}}//namespace
#endif


