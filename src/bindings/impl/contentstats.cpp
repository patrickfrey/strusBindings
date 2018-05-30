/*
 * Copyright (c) 2018 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/contentstats.hpp"
#include "strus/lib/detector_std.hpp"
#include "strus/lib/contentstats_std.hpp"
#include "strus/documentClassDetectorInterface.hpp"
#include "valueVariantWrap.hpp"
#include "impl/value/analyzerIntrospection.hpp"
#include "impl/value/featureFuncDef.hpp"
#include "strus/base/local_ptr.hpp"

using namespace strus;
using namespace strus::bindings;

ContentStatisticsImpl::ContentStatisticsImpl( const ObjectRef& trace_, const ObjectRef& errorhnd_, const TextProcessorInterface* textproc_)
	:m_errorhnd_impl(errorhnd_),m_trace_impl(trace_),m_contentstats_impl(),m_detector_impl(),m_textproc(textproc_)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	DocumentClassDetectorInterface* detector;
	m_detector_impl.resetOwnership( detector=strus::createDetector_std( m_textproc, errorhnd), "Detector");
	if (!m_detector_impl.get())
	{
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
	m_contentstats_impl.resetOwnership( strus::createContentStatistics_std( m_textproc, detector, errorhnd), "ContentStats");
	if (!m_contentstats_impl.get())
	{
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
}

void ContentStatisticsImpl::addLibraryElement(
		const std::string& type,
		const std::string& regex,
		const ValueVariant& priority_,
		const ValueVariant& minLength_,
		const ValueVariant& maxLength_,
		const ValueVariant& tokenizer,
		const ValueVariant& normalizers)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	ContentStatisticsInterface* contentstats = m_contentstats_impl.getObject<ContentStatisticsInterface>();
	int priority = papuga_ValueVariant_defined( &priority_) ? ValueVariantWrap::toint( priority_) : 0;
	int minLength = papuga_ValueVariant_defined( &minLength_) ? ValueVariantWrap::toint( minLength_) : -1;
	int maxLength = papuga_ValueVariant_defined( &maxLength_) ? ValueVariantWrap::toint( maxLength_) : -1;
	FeatureFuncDef funcdef( m_textproc, tokenizer, normalizers, errorhnd);
	contentstats->addLibraryElement( type, regex, priority, minLength, maxLength, funcdef.tokenizer.get(), funcdef.normalizers);
	funcdef.release();
}

ContentStatisticsCollectorImpl* ContentStatisticsImpl::createCollector() const
{
	return new ContentStatisticsCollectorImpl( m_trace_impl, m_contentstats_impl, m_detector_impl, m_errorhnd_impl, m_textproc);
}


Struct ContentStatisticsImpl::introspection( const ValueVariant& arg) const
{
	Struct rt;
	std::vector<std::string> path;
	if (papuga_ValueVariant_defined( &arg))
	{
		path = Deserializer::getStringList( arg);
	}
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const ContentStatisticsInterface* contentstats = m_contentstats_impl.getObject<ContentStatisticsInterface>();

	strus::local_ptr<IntrospectionBase> ictx( new ContentStatisticsIntrospection( errorhnd, contentstats));
	ictx->getPathContent( rt.serialization, path);
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT( "failed to serialize introspection: %s"), errorhnd->fetchError());
	}
	rt.release();
	return rt;
}


ContentStatisticsCollectorImpl::ContentStatisticsCollectorImpl( const ObjectRef& trace_, const ObjectRef& contentstat_, const ObjectRef& detector_, const ObjectRef& errorhnd_, const TextProcessorInterface* textproc_)
	:m_errorhnd_impl(errorhnd_),m_trace_impl(trace_),m_contentstats_impl(contentstat_),m_detector_impl(detector_),m_context_impl(),m_textproc(textproc_)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	ContentStatisticsInterface* contentstats = m_contentstats_impl.getObject<ContentStatisticsInterface>();
	m_context_impl.resetOwnership( contentstats->createContext(), "ContentStatsCollector");
	if (!m_context_impl.get()) throw strus::runtime_error( "%s", errorhnd->fetchError());
}

void ContentStatisticsCollectorImpl::putContent(
			const std::string& docid,
			const std::string& content,
			const ValueVariant& dclass)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	ContentStatisticsContextInterface* context = m_context_impl.getObject<ContentStatisticsContextInterface>();
	analyzer::DocumentClass documentClass;
	if (papuga_ValueVariant_defined( &dclass))
	{
		documentClass = Deserializer::getDocumentClass( dclass);
	}
	context->putContent( docid, content, documentClass);
	if (errorhnd->hasError()) throw strus::runtime_error( "%s", errorhnd->fetchError());
}

analyzer::ContentStatisticsResult* ContentStatisticsCollectorImpl::statistics()
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	ContentStatisticsContextInterface* context = m_context_impl.getObject<ContentStatisticsContextInterface>();
	strus::local_ptr<analyzer::ContentStatisticsResult> rt( new analyzer::ContentStatisticsResult( context->statistics()));
	if (errorhnd->hasError()) throw strus::runtime_error( "%s", errorhnd->fetchError());
	return rt.release();
}

int ContentStatisticsCollectorImpl::nofDocuments() const
{
	const ContentStatisticsContextInterface* context = m_context_impl.getObject<ContentStatisticsContextInterface>();
	return context->nofDocuments();
}

