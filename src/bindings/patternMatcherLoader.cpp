/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "patternMatcherLoader.hpp"
#include "private/internationalization.hpp"
#include "expressionBuilder.hpp"
#include "deserializer.hpp"
#include "strus/base/fileio.hpp"
#include "strus/lib/pattern_serialize.hpp"
#include "strus/lib/analyzer_prgload_std.hpp"

using namespace strus;
using namespace strus::bindings;

PatternMatcherPostProc bindings::loadPatternMatcherPostProc(
	const TextProcessorInterface* textproc,
	const std::string& patternMatcherModule,
	const papuga_ValueVariant& lexems,
	const papuga_ValueVariant& patterns,
	ErrorBufferInterface* errorhnd)
{
	PatternMatcherPostProc rt;
	const PatternMatcherInterface* matcher = textproc->getPatternMatcher( patternMatcherModule);
	if (!matcher) throw strus::runtime_error(_TXT("failed to load matcher module: %s"), errorhnd->fetchError());
	const PatternTermFeederInterface* feeder = textproc->getPatternTermFeeder();
	rt.matcher.reset( matcher->createInstance());
	rt.feeder.reset( feeder->createInstance());
	if (!rt.feeder.get()) throw strus::runtime_error(_TXT("failed to create pattern feeder: %s"), errorhnd->fetchError());
	if (!rt.matcher.get()) throw strus::runtime_error(_TXT("failed to create pattern matcher: %s"), errorhnd->fetchError());

	PostProcPatternExpressionBuilder expressionBuilder( rt.matcher.get(), rt.feeder.get(), errorhnd);
	std::vector<std::string> lexemList = Deserializer::getStringList( lexems);
	std::vector<std::string>::const_iterator si = lexemList.begin(), se = lexemList.end();
	for (; si != se; ++si)
	{
		expressionBuilder.defineLexem( *si);
	}
	Deserializer::buildPatterns( expressionBuilder, patterns, errorhnd);
	return rt;
}

PatternMatcherPostProc bindings::loadPatternMatcherPostProcFromFile(
	const TextProcessorInterface* textproc,
	const std::string& patternMatcherModule,
	const std::string& filename,
	ErrorBufferInterface* errorhnd)
{
	PatternMatcherPostProc rt;
	const PatternMatcherInterface* matcher = textproc->getPatternMatcher( patternMatcherModule);
	if (!matcher) throw strus::runtime_error(_TXT("failed to load matcher module: %s"), errorhnd->fetchError());
	const PatternTermFeederInterface* feeder = textproc->getPatternTermFeeder();
	rt.matcher.reset( matcher->createInstance());
	rt.feeder.reset( feeder->createInstance());
	if (!rt.feeder.get()) throw strus::runtime_error(_TXT("failed to create pattern feeder: %s"), errorhnd->fetchError());
	if (!rt.matcher.get()) throw strus::runtime_error(_TXT("failed to create pattern matcher: %s"), errorhnd->fetchError());
	if (!strus::load_PatternMatcher_programfile( textproc, rt.feeder.get(), rt.matcher.get(), filename, errorhnd))
	{
		throw strus::runtime_error(_TXT("failed to load pattern matcher from source file: %s"), errorhnd->fetchError());
	}
	return rt;
}

