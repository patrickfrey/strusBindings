/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Interface for introspection of the root object
#ifndef _STRUS_BINDING_IMPL_VALUE_CONTEXT_INTROSPECTION_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_VALUE_CONTEXT_INTROSPECTION_HPP_INCLUDED
#include "introspectionBase.hpp"
#include "strus/textProcessorInterface.hpp"
#include "strus/queryProcessorInterface.hpp"
#include "strus/moduleLoaderInterface.hpp"
#include "strus/rpcClientInterface.hpp"
#include "strus/analyzerObjectBuilderInterface.hpp"
#include "strus/storageObjectBuilderInterface.hpp"
#include "traceProxy.hpp"

namespace strus {
namespace bindings {

class ContextIntrospection
	:public IntrospectionBase
{
public:
	ContextIntrospection(
			ErrorBufferInterface* errorhnd_,
			const ModuleLoaderInterface* moduleloader_,
			const TraceProxy* trace_,
			const StorageObjectBuilderInterface* storage_,
			const AnalyzerObjectBuilderInterface* analyzer_,
			const RpcClientInterface* rpc_,
			int threads_)
		:m_errorhnd(errorhnd_)
		,m_moduleloader(moduleloader_)
		,m_trace(trace_)
		,m_rpc(rpc_)
		,m_textproc(analyzer_?analyzer_->getTextProcessor():0)
		,m_queryproc(storage_?storage_->getQueryProcessor():0)
		,m_threads(threads_)
		{}

	ContextIntrospection( const ContextIntrospection& o)
		:m_errorhnd(o.m_errorhnd)
		,m_moduleloader(o.m_moduleloader)
		,m_trace(o.m_trace)
		,m_rpc(o.m_rpc)
		,m_textproc(o.m_textproc)
		,m_queryproc(o.m_queryproc)
		,m_threads(o.m_threads)
		{}

	virtual void serialize( papuga_Serialization& serialization) const;
	virtual IntrospectionBase* open( const std::string& name) const;
	virtual std::vector<std::string> list() const;

private:
	ErrorBufferInterface* m_errorhnd;
	const ModuleLoaderInterface* m_moduleloader;
	const TraceProxy* m_trace;
	const RpcClientInterface* m_rpc;
	const TextProcessorInterface* m_textproc;
	const QueryProcessorInterface* m_queryproc;
	int m_threads;
};

}}//namespace
#endif

