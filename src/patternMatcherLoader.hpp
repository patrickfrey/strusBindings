/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_PATTERN_MATCHER_LOADER_HPP_INCLUDED
#define _STRUS_BINDING_PATTERN_MATCHER_LOADER_HPP_INCLUDED
#include "strus/bindings/valueVariant.hpp"
#include "strus/bindings/serialization.hpp"
#include "strus/patternLexerInterface.hpp"
#include "strus/patternLexerInstanceInterface.hpp"
#include "strus/patternTermFeederInterface.hpp"
#include "strus/patternTermFeederInstanceInterface.hpp"
#include "strus/patternMatcherInterface.hpp"
#include "strus/patternMatcherInstanceInterface.hpp"
#include "strus/textProcessorInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/reference.hpp"
#include <string>

namespace strus {
namespace bindings {

struct PatternMatcherPostProc
{
	Reference<PatternMatcherInstanceInterface> matcher;
	Reference<PatternTermFeederInstanceInterface> feeder;

	PatternMatcherPostProc()
		:matcher(),feeder(){}
	PatternMatcherPostProc( const Reference<PatternMatcherInstanceInterface>& matcher_, const Reference<PatternTermFeederInstanceInterface>& feeder_)
		:matcher(matcher_),feeder(feeder_){}
	PatternMatcherPostProc( const PatternMatcherPostProc& o)
		:matcher(o.matcher),feeder(o.feeder){}

	void release()
	{
		matcher.release();
		feeder.release();
	}
};

PatternMatcherPostProc loadPatternMatcherPostProc(
	const TextProcessorInterface* textproc,
	const std::string& patternMatcherModule,
	const ValueVariant& lexems,
	const ValueVariant& patterns,
	ErrorBufferInterface* errorhnd);

PatternMatcherPostProc loadPatterMatcherPostProcFromFile(
	const TextProcessorInterface* textproc,
	const std::string& patternMatcherModule,
	const std::string& filename,
	ErrorBufferInterface* errorhnd);

}}//namespace
#endif


