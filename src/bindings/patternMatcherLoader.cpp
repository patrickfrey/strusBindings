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
	std::string filepath( textproc->getResourcePath( filename));
	if (filepath.empty())
	{
		throw strus::runtime_error(_TXT("path of file '%s' not found: %s"), filename.c_str(), errorhnd->fetchError());
	}
	std::string content;
	unsigned int ec = readFile( filepath, content);
	if (ec) throw strus::runtime_error(_TXT("failed to read serialized patterns from file '%s': %s"), filepath.c_str(), ::strerror(ec));
	if (!loadPatternMatcherFromSerialization( content, rt.feeder.get(), rt.matcher.get(), errorhnd))
	{
		throw strus::runtime_error(_TXT("failed to load pattern matcher from serialization: %s"), errorhnd->fetchError());
	}
	return rt;
}

