/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "bindingObjectsImpl.hpp"
#include "deserializer.hpp"
#include "expressionBuilder.hpp"
#include "strus/strus.hpp"
#include "strus/lib/rpc_client.hpp"
#include "strus/lib/rpc_client_socket.hpp"
#include "strus/lib/module.hpp"
#include "strus/lib/storage_objbuild.hpp"
#include "strus/lib/error.hpp"
#include "strus/rpcClientInterface.hpp"
#include "strus/rpcClientMessagingInterface.hpp"
#include "strus/moduleLoaderInterface.hpp"
#include "strus/storageObjectBuilderInterface.hpp"
#include "strus/analyzerObjectBuilderInterface.hpp"
#include "strus/statisticsIteratorInterface.hpp"
#include "strus/statisticsViewerInterface.hpp"
#include "strus/statisticsBuilderInterface.hpp"
#include "strus/metaDataRestrictionInterface.hpp"
#include "strus/metaDataReaderInterface.hpp"
#include "strus/postingIteratorInterface.hpp"
#include "strus/scalarFunctionInterface.hpp"
#include "strus/scalarFunctionParserInterface.hpp"
#include "strus/documentAnalyzerInterface.hpp"
#include "strus/documentAnalyzerContextInterface.hpp"
#include "strus/queryAnalyzerInterface.hpp"
#include "strus/queryAnalyzerContextInterface.hpp"
#include "strus/patternMatcherInterface.hpp"
#include "strus/patternMatcherInstanceInterface.hpp"
#include "strus/patternTermFeederInterface.hpp"
#include "strus/patternTermFeederInstanceInterface.hpp"
#include "strus/vectorStorageClientInterface.hpp"
#include "strus/vectorStorageSearchInterface.hpp"
#include "strus/vectorStorageTransactionInterface.hpp"
#include "strus/vectorStorageInterface.hpp"
#include "strus/textProcessorInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/base/configParser.hpp"
#include "strus/base/utf8.hpp"
#include "strus/base/symbolTable.hpp"
#include "strus/base/fileio.hpp"
#include "internationalization.hpp"
#include "utils.hpp"
#include "traceUtils.hpp"
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <memory>
#include <cstdlib>
#include <limits>

using namespace strus;
using namespace strus::bindings;

VectorStorageSearcherImpl::VectorStorageSearcherImpl( const HostObjectReference& storageref, const HostObjectReference& trace, const Index& range_from, const Index& range_to, const HostObjectReference& errorhnd_)
	:m_errorhnd_impl(errorhnd_)
	,m_searcher_impl(ReferenceDeleter<VectorStorageSearchInterface>::function)
	,m_trace_impl( trace)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	VectorStorageClientInterface* storage = (VectorStorageClientInterface*)storageref.get();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	m_searcher_impl.reset( storage->createSearcher( range_from, range_to));
	if (!m_searcher_impl.get())
	{
		throw strus::runtime_error( _TXT("failed to create vector storage searcher: %s"), errorhnd->fetchError());
	}
}

VectorStorageSearcherImpl::VectorStorageSearcherImpl( const VectorStorageSearcher& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_searcher_impl(o.m_searcher_impl)
	,m_trace_impl(o.m_trace_impl)
{}

std::vector<VecRank> VectorStorageSearcherImpl::findSimilar( const std::vector<double>& vec, unsigned int maxNofResults) const
{
	VectorStorageSearchInterface* searcher = (VectorStorageSearchInterface*)m_searcher_impl.get();
	if (!searcher) throw strus::runtime_error( _TXT("calling vector storage searcher method after close"));

	std::vector<VectorStorageSearchInterface::Result> res = searcher->findSimilar( vec, maxNofResults);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("error in find similar features of vector: %s"), errorhnd->fetchError());
	}
	std::vector<VecRank> rt;
	std::vector<VectorStorageSearchInterface::Result>::const_iterator ri = res.begin(), re = res.end();
	for (; ri != re; ++ri)
	{
		rt.push_back( VecRank( ri->featidx(), ri->weight()));
	}
	return rt;
}

std::vector<VecRank> VectorStorageSearcherImpl::findSimilarFromSelection( const IndexVector& featidxlist, const FloatVector& vec, unsigned int maxNofResults) const
{
	VectorStorageSearchInterface* searcher = (VectorStorageSearchInterface*)m_searcher_impl.get();
	if (!searcher) throw strus::runtime_error( _TXT("calling vector storage searcher method after close"));

	std::vector<VectorStorageSearchInterface::Result> res = searcher->findSimilarFromSelection( featidxlist, vec, maxNofResults);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("error in find similar features of vector: %s"), errorhnd->fetchError());
	}
	std::vector<VecRank> rt;
	std::vector<VectorStorageSearchInterface::Result>::const_iterator ri = res.begin(), re = res.end();
	for (; ri != re; ++ri)
	{
		rt.push_back( VecRank( ri->featidx(), ri->weight()));
	}
	return rt;
}

void VectorStorageSearcherImpl::close()
{
	if (!m_searcher_impl.get()) throw strus::runtime_error( _TXT("calling storage searcher method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	bool preverr = errorhnd->hasError();
	m_searcher_impl.reset();
	if (!preverr && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("error detected after calling storage searcher close: %s"), errorhnd->fetchError());
	}
}

VectorStorageClientImpl::VectorStorageClientImpl( const VectorStorageClientImpl& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_vector_storage_impl(o.m_vector_storage_impl)
	,m_config(o.m_config)
{}

void VectorStorageClientImpl::close()
{
	if (!m_vector_storage_impl.get()) throw strus::runtime_error( _TXT("calling vector storage client method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	bool preverr = errorhnd->hasError();
	m_vector_storage_impl.reset();
	if (!preverr && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("error detected after calling vector storage client close: %s"), errorhnd->fetchError());
	}
}

VectorStorageSearcherImpl VectorStorageClientImpl::createSearcher( const Index& range_from, const Index& range_to) const
{
	return VectorStorageSearcherImpl( m_vector_storage_impl, m_trace_impl, range_from, range_to, m_errorhnd_impl);
}

VectorStorageTransactionImpl VectorStorageClientImpl::createTransaction()
{
	return VectorStorageTransaction( m_objbuilder_impl, m_vector_storage_impl, m_trace_impl, m_errorhnd_impl, m_config);
}

std::vector<std::string> VectorStorageClientImpl::conceptClassNames() const
{
	VectorStorageClientInterface* storage = (VectorStorageClientInterface*)m_vector_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));
	std::vector<std::string> rt = storage->conceptClassNames();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get concept class names: %s"), errorhnd->fetchError());
	}
	return rt;
}

std::vector<Index> VectorStorageClientImpl::conceptFeatures( const std::string& conceptClass, const Index& conceptid) const
{
	VectorStorageClientInterface* storage = (VectorStorageClientInterface*)m_vector_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));
	std::vector<Index> res = storage->conceptFeatures( conceptClass, conceptid);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get concept features: %s"), errorhnd->fetchError());
	}
	std::vector<Index> rt;
	std::vector<Index>::const_iterator ri = res.begin(), re = res.end();
	for (; ri != re; ++ri)
	{
		rt.push_back( *ri);
	}
	return rt;
}

unsigned int VectorStorageClientImpl::nofConcepts( const std::string& conceptClass) const
{
	VectorStorageClientInterface* storage = (VectorStorageClientInterface*)m_vector_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	unsigned int rt = storage->nofConcepts( conceptClass);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get number of concepts: %s"), errorhnd->fetchError());
	}
	return rt;
}

std::vector<Index> VectorStorageClientImpl::featureConcepts( const std::string& conceptClass, const Index& index) const
{
	VectorStorageClientInterface* storage = (VectorStorageClientInterface*)m_vector_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	std::vector<Index> res = storage->featureConcepts( conceptClass, index);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get feature concepts: %s"), errorhnd->fetchError());
	}
	std::vector<Index> rt;
	std::vector<Index>::const_iterator ri = res.begin(), re = res.end();
	for (; ri != re; ++ri)
	{
		rt.push_back( *ri);
	}
	return rt;
}

std::vector<double> VectorStorageClientImpl::featureVector( const Index& index) const
{
	VectorStorageClientInterface* storage = (VectorStorageClientInterface*)m_vector_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	std::vector<double> rt = storage->featureVector( index);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get feature vector of %u: %s"), index, errorhnd->fetchError());
	}
	return rt;
}

std::string VectorStorageClientImpl::featureName( const Index& index) const
{
	VectorStorageClientInterface* storage = (VectorStorageClientInterface*)m_vector_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	std::string rt = storage->featureName( index);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get feature name of %u: %s"), index, errorhnd->fetchError());
	}
	return rt;
}

Index VectorStorageClientImpl::featureIndex( const std::string& name) const
{
	VectorStorageClientInterface* storage = (VectorStorageClientInterface*)m_vector_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	Index rt = storage->featureIndex( name);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get feature name of %s: %s"), name.c_str(), errorhnd->fetchError());
	}
	return rt;
}

unsigned int VectorStorageClientImpl::nofFeatures() const
{
	VectorStorageClientInterface* storage = (VectorStorageClientInterface*)m_vector_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	unsigned int rt = storage->nofFeatures();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get number of features defined: %s"), errorhnd->fetchError());
	}
	return rt;
}

std::string VectorStorageClientImpl::config() const
{
	VectorStorageClientInterface* storage = (VectorStorageClientInterface*)m_vector_storage_impl.get();
	if (!storage) throw strus::runtime_error( _TXT("calling vector storage client method after close"));

	std::string rt = storage->config();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get the storage configuration string: %s"), errorhnd->fetchError());
	}
	return rt;
}

VectorStorageClientImpl::VectorStorageClientImpl( const HostObjectReference& objbuilder, const HostObjectReference& trace, const HostObjectReference& errorhnd_, const std::string& config)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl( trace)
	,m_objbuilder_impl( objbuilder)
	,m_vector_storage_impl(ReferenceDeleter<VectorStorageClientInterface>::function)
	,m_config(config)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StorageObjectBuilderInterface* objBuilder = (const StorageObjectBuilderInterface*)m_objbuilder_impl.get();

	m_vector_storage_impl.reset( createVectorStorageClient( objBuilder, errorhnd, config));
	if (!m_vector_storage_impl.get())
	{
		throw strus::runtime_error( _TXT("failed to create vector storage client: %s"), errorhnd->fetchError());
	}
}

VectorStorageTransactionImpl::VectorStorageTransaction( const VectorStorageTransaction& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_vector_storage_impl(o.m_vector_storage_impl)
	,m_vector_transaction_impl(o.m_vector_transaction_impl)
{}

void VectorStorageTransactionImpl::addFeature( const std::string& name, const std::vector<double>& vec)
{
	VectorStorageTransactionInterface* transaction = (VectorStorageTransactionInterface*)m_vector_transaction_impl.get();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (!transaction)
	{
		StorageClientInterface* storage = (StorageClientInterface*)m_vector_storage_impl.get();
		m_vector_transaction_impl.reset( (StorageTransactionInterface*)storage->createTransaction());
		if (!m_vector_transaction_impl.get()) throw strus::runtime_error( _TXT("failed to create transaction for insert document: %s"), errorhnd->fetchError());
		transaction = (VectorStorageTransactionInterface*)m_vector_transaction_impl.get();
	}
	transaction->addFeature( name, vec);
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to add feature to vector storage builder: %s"), errorhnd->fetchError());
	}
}

void VectorStorageTransactionImpl::defineFeatureConceptRelation( const std::string& relationTypeName, const Index& featidx, const Index& conidx)
{
	VectorStorageTransactionInterface* transaction = (VectorStorageTransactionInterface*)m_vector_transaction_impl.get();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (!transaction)
	{
		StorageClientInterface* storage = (StorageClientInterface*)m_vector_storage_impl.get();
		m_vector_transaction_impl.reset( (StorageTransactionInterface*)storage->createTransaction());
		if (!m_vector_transaction_impl.get()) throw strus::runtime_error( _TXT("failed to create transaction for insert document: %s"), errorhnd->fetchError());
		transaction = (VectorStorageTransactionInterface*)m_vector_transaction_impl.get();
	}
	transaction->defineFeatureConceptRelation( relationTypeName, featidx, conidx);
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to add feature to vector storage builder: %s"), errorhnd->fetchError());
	}
}

bool VectorStorageTransactionImpl::commit()
{
	VectorStorageTransactionInterface* transaction = (VectorStorageTransactionInterface*)m_vector_transaction_impl.get();
	if (!transaction) throw strus::runtime_error( _TXT("calling vector storage builder method after close"));

	bool rt = transaction->commit();
	if (!rt)
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		if (errorhnd->hasError())
		{
			throw strus::runtime_error(_TXT("failed to complete vector storage building (done): %s"), errorhnd->fetchError());
		}
	}
	return rt;
}

void VectorStorageTransactionImpl::rollback()
{
	VectorStorageTransactionInterface* transaction = (VectorStorageTransactionInterface*)m_vector_transaction_impl.get();
	if (!transaction) throw strus::runtime_error( _TXT("calling vector storage builder method after close"));
	transaction->rollback();
}

void VectorStorageTransactionImpl::close()
{
	if (!m_vector_transaction_impl.get()) throw strus::runtime_error( _TXT("calling vector storage builder method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	bool preverr = errorhnd->hasError();
	m_vector_transaction_impl.reset();
	if (!preverr && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("error detected after calling vector storage client close: %s"), errorhnd->fetchError());
	}
}

VectorStorageTransactionImpl::VectorStorageTransactionImpl( const HostObjectReference& objbuilder, const HostObjectReference& storageref, const HostObjectReference& trace, const HostObjectReference& errorhnd_, const std::string& config)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl(trace)
	,m_objbuilder_impl(objbuilder)
	,m_vector_storage_impl(storageref)
	,m_vector_transaction_impl(ReferenceDeleter<VectorStorageTransactionInterface>::function)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	VectorStorageClientInterface* storage = (VectorStorageClientInterface*)m_vector_storage_impl.get();

	m_vector_transaction_impl.reset( storage->createTransaction());
	if (!m_vector_transaction_impl.get())
	{
		throw strus::runtime_error( _TXT("failed to create vector storage transaction: %s"), errorhnd->fetchError());
	}
}


StatisticsIteratorImpl::StatisticsIteratorImpl( const StatisticsIteratorImpl& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_storage_impl(o.m_storage_impl)
	,m_iter_impl(o.m_iter_impl){}

StatisticsIteratorImpl::StatisticsIteratorImpl( const HostObjectReference& objbuilder, const HostObjectReference& trace, const HostObjectReference& errorhnd_, const HostObjectReference& storage_, const HostObjectReference& iter_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl(trace)
	,m_objbuilder_impl(objbuilder)
	,m_storage_impl(storage_)
	,m_iter_impl(iter_)
{}

std::string StatisticsIteratorImpl::getNext()
{
	StatisticsIteratorInterface* iter = (StatisticsIteratorInterface*)m_iter_impl.get();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const char* outmsg;
	std::size_t outmsgsize;
	if (!iter->getNext( outmsg, outmsgsize))
	{
		if (errorhnd->hasError())
		{
			throw strus::runtime_error( _TXT("error fetching statistics message: %s"), errorhnd->fetchError());
		}
	}
	return std::string( outmsg, outmsgsize);
}

StatisticsProcessorImpl::StatisticsProcessorImpl( const HostObjectReference& objbuilder_, const HostObjectReference& trace_, const std::string& name_, const HostObjectReference& errorhnd_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl(trace_)
	,m_objbuilder_impl(objbuilder_)
	,m_statsproc(0)
{
	const StorageObjectBuilderInterface* objBuilder = (const StorageObjectBuilderInterface*)m_objbuilder_impl.get();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	m_statsproc = objBuilder->getStatisticsProcessor( name_);
	if (!m_statsproc)
	{
		if (errorhnd->hasError())
		{
			throw strus::runtime_error(_TXT( "error getting statistics message processor: %s"), errorhnd->fetchError());
		}
		throw strus::runtime_error(_TXT( "error statistics message processor not defined"));
	}
}

ValueVariant StatisticsProcessorImpl::decode( const std::string& blob) const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StatisticsProcessorInterface* proc = (const StatisticsProcessorInterface*)m_statsproc;
	std::auto_ptr<StatisticsViewerInterface> viewer( proc->createViewer( blob.c_str(), blob.size()));
	std::vector<DocumentFrequencyChange> dflist;
	StatisticsViewerInterface::DocumentFrequencyChange rec;
	while (viewer->nextDfChange( rec))
	{
		dflist.push_back( DocumentFrequencyChange( rec.type, rec.value, rec.increment));
	}
	int nofdocs = viewer->nofDocumentsInsertedChange();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT( "error statistics message structure from blob: %s"), errorhnd->fetchError());
	}
	return StatisticsMessage( dflist, nofdocs);
}

std::string StatisticsProcessorImpl::encode( const ValueVariant& msg) const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StatisticsProcessorInterface* proc = (const StatisticsProcessorInterface*)m_statsproc;
	StatisticsProcessorInterface::BuilderOptions options;
	std::auto_ptr<StatisticsBuilderInterface> builder( proc->createBuilder( options));
	std::vector<DocumentFrequencyChange>::const_iterator
			dfi = msg.documentFrequencyChangeList().begin(),
			dfe = msg.documentFrequencyChangeList().end();
	for (; dfi != dfe; ++dfi)
	{
		builder->addDfChange( dfi->type().c_str(), dfi->value().c_str(), dfi->increment());
	}
	builder->setNofDocumentsInsertedChange( msg.nofDocumentsInsertedChange());
	std::string rt;
	const char* blk;
	std::size_t blksize;
	if (builder->fetchMessage( blk, blksize))
	{
		rt.append( blk, blksize);
	}
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT( "error creating blob from statistics message structure: %s"), errorhnd->fetchError());
	}
	return rt;
}


DocumentBrowserImpl::DocumentBrowserImpl( const DocumentBrowserImpl& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_trace_impl(o.m_trace_impl)
	,m_objbuilder_impl(o.m_objbuilder_impl)
	,m_storage_impl(o.m_storage_impl)
	,m_restriction_impl(o.m_restriction_impl)
	,m_postingitr_impl(o.m_postingitr_impl)
	,m_attributereader_impl(o.m_attributereader_impl)
	,m_docno(o.m_docno)
{}

DocumentBrowserImpl::DocumentBrowserImpl( const HostObjectReference& objbuilder_impl_, const HostObjectReference& trace_impl_, const HostObjectReference& storage_impl_, const HostObjectReference& errorhnd_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl(trace_impl_)
	,m_objbuilder_impl(objbuilder_impl_)
	,m_storage_impl(storage_impl_)
	,m_restriction_impl( ReferenceDeleter<MetaDataRestrictionInterface>::function)
	,m_postingitr_impl( ReferenceDeleter<PostingIteratorInterface>::function)
	,m_attributereader_impl( ReferenceDeleter<AttributeReaderInterface>::function)
	,m_docno(0)
{
	const StorageClientInterface* storage = (const StorageClientInterface*)m_storage_impl.get();
	m_restriction_impl.reset( storage->createMetaDataRestriction());
	if (!m_restriction_impl.get())
	{
		throw strus::runtime_error( _TXT("failed to create meta data restriction interface for browsing documents"));
	}
}

void DocumentBrowserImpl::addMetaDataRestrictionCondition(
		const char* compareOp, const std::string& name,
		const Variant& value, bool newGroup)
{
	MetaDataRestrictionInterface* restriction = (MetaDataRestrictionInterface*)m_restriction_impl.get();
	if (!restriction)
	{
		throw strus::runtime_error( _TXT("it is not allowed to add more restrictions to a document browser after the first call of next()"));
	}
	MetaDataRestrictionInterface::CompareOperator cmpop = getCompareOp( compareOp);
	restriction->addCondition( cmpop, name, numericVariant(value), newGroup);
}

void DocumentBrowserImpl::addMetaDataRestrictionCondition(
		const char* compareOp, const std::string& name,
		double value, bool newGroup)
{
	addMetaDataRestrictionCondition( compareOp, name, Variant(value), newGroup);
}

void DocumentBrowserImpl::addMetaDataRestrictionCondition(
		const char* compareOp, const std::string& name,
		unsigned int value, bool newGroup)
{
	addMetaDataRestrictionCondition( compareOp, name, Variant(value), newGroup);
}

void DocumentBrowserImpl::addMetaDataRestrictionCondition(
		const char* compareOp, const std::string& name,
		int value, bool newGroup)
{
	addMetaDataRestrictionCondition( compareOp, name, Variant(value), newGroup);
}

Index DocumentBrowserImpl::skipDoc( const Index& docno_)
{
	if (!m_postingitr_impl.get())
	{
		const StorageClientInterface* storage = (const StorageClientInterface*)m_storage_impl.get();
		MetaDataRestrictionInterface* restriction = (MetaDataRestrictionInterface*)m_restriction_impl.get();
		m_postingitr_impl.reset( storage->createBrowsePostingIterator( restriction, 1));
		if (!m_postingitr_impl.get())
		{
			throw strus::runtime_error( _TXT("failed to create posting iterator for document browser"));
		}
	}
	PostingIteratorInterface* itr = (PostingIteratorInterface*)m_postingitr_impl.get();
	return m_docno = itr->skipDoc( docno_);
}

std::string DocumentBrowserImpl::attribute( const std::string& name)
{
	if (m_docno)
	{
		if (!m_attributereader_impl.get())
		{
			const StorageClientInterface* storage = (const StorageClientInterface*)m_storage_impl.get();
			m_attributereader_impl.reset( storage->createAttributeReader());
			if (!m_attributereader_impl.get())
			{
				throw strus::runtime_error( _TXT("failed to create attribute reader for document browser"));
			}
		}
		const AttributeReaderInterface* reader = (AttributeReaderInterface*)m_attributereader_impl.get();
		Index elemhnd = reader->elementHandle( name.c_str());
		if (!elemhnd)
		{
			throw strus::runtime_error( _TXT("document attribute name %s is not defined"), name.c_str());
		}
		return reader->getValue( elemhnd);
	}
	else
	{
		return std::string();
	}
}


static ErrorBufferInterface* createErrorBuffer_( unsigned int maxNofThreads)
{
	ErrorBufferInterface* errorhnd = createErrorBuffer_standard( 0, maxNofThreads);
	if (!errorhnd)
	{
		throw strus::runtime_error( _TXT("failed to create error buffer object: %s"), errorhnd->fetchError());
	}
	return errorhnd;
}

static ModuleLoaderInterface* createModuleLoader_( ErrorBufferInterface* errorhnd)
{
	ModuleLoaderInterface* rt = createModuleLoader( errorhnd);
	if (!rt)
	{
		throw strus::runtime_error( _TXT("failed to create module loader object: %s"), errorhnd->fetchError());
	}
	return rt;
}

ContextImpl::ContextImpl()
	:m_errorhnd_impl( ReferenceDeleter<ErrorBufferInterface>::function)
	,m_moduleloader_impl( ReferenceDeleter<ModuleLoaderInterface>::function)
	,m_rpc_impl( ReferenceDeleter<RpcClientInterface>::function)
	,m_trace_impl( ReferenceDeleter<TraceProxy>::function)
	,m_storage_objbuilder_impl( ReferenceDeleter<StorageObjectBuilderInterface>::function)
	,m_analyzer_objbuilder_impl( ReferenceDeleter<StorageObjectBuilderInterface>::function)
	,m_textproc(0)
{
	ErrorBufferInterface* errorhnd = createErrorBuffer_( 0);
	m_errorhnd_impl.reset( errorhnd);
	ModuleLoaderInterface* moduleLoader = createModuleLoader_( errorhnd);
	m_moduleloader_impl.reset( moduleLoader);
}

ContextImpl::ContextImpl( unsigned int maxNofThreads)
	:m_errorhnd_impl( ReferenceDeleter<ErrorBufferInterface>::function)
	,m_moduleloader_impl( ReferenceDeleter<ModuleLoaderInterface>::function)
	,m_rpc_impl( ReferenceDeleter<RpcClientInterface>::function)
	,m_trace_impl( ReferenceDeleter<TraceProxy>::function)
	,m_storage_objbuilder_impl( ReferenceDeleter<StorageObjectBuilderInterface>::function)
	,m_analyzer_objbuilder_impl( ReferenceDeleter<StorageObjectBuilderInterface>::function)
	,m_textproc(0)
{
	ErrorBufferInterface* errorhnd = createErrorBuffer_( maxNofThreads);
	m_errorhnd_impl.reset( errorhnd);
	ModuleLoaderInterface* moduleLoader = createModuleLoader_( errorhnd);
	m_moduleloader_impl.reset( moduleLoader);
}

ContextImpl::ContextImpl( unsigned int maxNofThreads, const std::string& tracecfg)
	:m_errorhnd_impl( ReferenceDeleter<ErrorBufferInterface>::function)
	,m_moduleloader_impl( ReferenceDeleter<ModuleLoaderInterface>::function)
	,m_rpc_impl( ReferenceDeleter<RpcClientInterface>::function)
	,m_trace_impl( ReferenceDeleter<TraceProxy>::function)
	,m_storage_objbuilder_impl( ReferenceDeleter<StorageObjectBuilderInterface>::function)
	,m_analyzer_objbuilder_impl( ReferenceDeleter<StorageObjectBuilderInterface>::function)
	,m_textproc(0)
{
	ErrorBufferInterface* errorhnd = createErrorBuffer_( maxNofThreads);
	m_errorhnd_impl.reset( errorhnd);
	ModuleLoaderInterface* moduleLoader = createModuleLoader_( errorhnd);
	m_moduleloader_impl.reset( moduleLoader);
	if (!tracecfg.empty())
	{
		m_trace_impl.reset( new TraceProxy( moduleLoader, tracecfg, errorhnd));
	}
}

ContextImpl::ContextImpl( const std::string& connectionstring)
	:m_errorhnd_impl( ReferenceDeleter<ErrorBufferInterface>::function)
	,m_moduleloader_impl( ReferenceDeleter<ModuleLoaderInterface>::function)
	,m_rpc_impl( ReferenceDeleter<RpcClientInterface>::function)
	,m_trace_impl( ReferenceDeleter<TraceProxy>::function)
	,m_storage_objbuilder_impl( ReferenceDeleter<StorageObjectBuilderInterface>::function)
	,m_analyzer_objbuilder_impl( ReferenceDeleter<StorageObjectBuilderInterface>::function)
	,m_textproc(0)
{
	ErrorBufferInterface* errorhnd = createErrorBuffer_( 0);
	m_errorhnd_impl.reset( errorhnd);

	std::auto_ptr<RpcClientMessagingInterface> messaging;
	messaging.reset( createRpcClientMessaging( connectionstring.c_str(), errorhnd));
	if (!messaging.get()) throw strus::runtime_error(_TXT("failed to create client messaging: %s"), errorhnd->fetchError());
	m_rpc_impl.reset( createRpcClient( messaging.get(), errorhnd));
	if (!m_rpc_impl.get()) throw strus::runtime_error(_TXT("failed to create rpc client: %s"), errorhnd->fetchError());
	(void)messaging.release();
}

ContextImpl::ContextImpl( const std::string& connectionstring, unsigned int maxNofThreads)
	:m_errorhnd_impl( ReferenceDeleter<ErrorBufferInterface>::function)
	,m_moduleloader_impl( ReferenceDeleter<ModuleLoaderInterface>::function)
	,m_rpc_impl( ReferenceDeleter<RpcClientInterface>::function)
	,m_trace_impl( ReferenceDeleter<TraceProxy>::function)
	,m_storage_objbuilder_impl( ReferenceDeleter<StorageObjectBuilderInterface>::function)
	,m_analyzer_objbuilder_impl( ReferenceDeleter<StorageObjectBuilderInterface>::function)
	,m_textproc(0)
{
	ErrorBufferInterface* errorhnd = createErrorBuffer_( maxNofThreads);
	m_errorhnd_impl.reset( errorhnd);

	std::auto_ptr<RpcClientMessagingInterface> messaging;
	messaging.reset( createRpcClientMessaging( connectionstring.c_str(), errorhnd));
	if (!messaging.get()) throw strus::runtime_error(_TXT("failed to create client messaging: %s"), errorhnd->fetchError());
	m_rpc_impl.reset( createRpcClient( messaging.get(), errorhnd));
	if (!m_rpc_impl.get()) throw strus::runtime_error(_TXT("failed to create rpc client: %s"), errorhnd->fetchError());
	(void)messaging.release();
}

ContextImpl::ContextImpl( const Context& o)
	:m_errorhnd_impl(o.m_errorhnd_impl)
	,m_moduleloader_impl(o.m_moduleloader_impl)
	,m_rpc_impl(o.m_rpc_impl)
	,m_trace_impl( o.m_trace_impl)
	,m_storage_objbuilder_impl(o.m_storage_objbuilder_impl)
	,m_analyzer_objbuilder_impl(o.m_analyzer_objbuilder_impl)
	,m_textproc(o.m_textproc)
{}

void ContextImpl::checkErrors() const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("unhandled error: %s"), errorhnd->fetchError());
	}
}

void ContextImpl::loadModule( const std::string& name_)
{
	if (!m_moduleloader_impl.get()) throw strus::runtime_error( _TXT("cannot load modules in RPC client mode"));
	if (m_storage_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to load modules after the first use of objects"));
	if (m_analyzer_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to load modules after the first use of objects"));
	ModuleLoaderInterface* moduleLoader = (ModuleLoaderInterface*)m_moduleloader_impl.get();
	if (!moduleLoader->loadModule( name_))
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error(_TXT("failed to load module: %s"), errorhnd->fetchError());
	}
}

void ContextImpl::addModulePath( const std::string& paths_)
{
	if (!m_moduleloader_impl.get()) throw strus::runtime_error( _TXT("cannot add a module path in RPC client mode"));
	if (m_storage_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to set the module search path after the first use of objects"));
	if (m_analyzer_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to set the module search path after the first use of objects"));
	ModuleLoaderInterface* moduleLoader = (ModuleLoaderInterface*)m_moduleloader_impl.get();
	moduleLoader->addModulePath( paths_);
}

void ContextImpl::addResourcePath( const std::string& paths_)
{
	if (!m_moduleloader_impl.get()) throw strus::runtime_error( _TXT("cannot add a resource path in RPC client mode"));
	if (m_storage_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to add a resource path after the first use of objects"));
	if (m_analyzer_objbuilder_impl.get()) throw strus::runtime_error( _TXT("tried to add a resource path after the first use of objects"));
	ModuleLoaderInterface* moduleLoader = (ModuleLoaderInterface*)m_moduleloader_impl.get();
	moduleLoader->addResourcePath( paths_);
}

StatisticsProcessorImpl ContextImpl::createStatisticsProcessor( const std::string& name)
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return StatisticsProcessor( m_storage_objbuilder_impl, m_trace_impl, name, m_errorhnd_impl);
}

void ContextImpl::initStorageObjBuilder()
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	StorageObjectBuilderInterface* storageObjectBuilder = 0;
	if (m_rpc_impl.get())
	{
		RpcClientInterface* client = (RpcClientInterface*)m_rpc_impl.get();
		storageObjectBuilder = client->createStorageObjectBuilder();
	}
	else if (m_moduleloader_impl.get())
	{
		ModuleLoaderInterface* moduleLoader = (ModuleLoaderInterface*)m_moduleloader_impl.get();
		storageObjectBuilder = moduleLoader->createStorageObjectBuilder();
	}
	else
	{
		throw strus::runtime_error( _TXT("bad state, no context initialized"));
	}
	if (!storageObjectBuilder)
	{
		throw strus::runtime_error(_TXT("failed to create storage object builder: %s"), errorhnd->fetchError());
	}
	TraceProxy* tp = (TraceProxy*)m_trace_impl.get();
	if (tp)
	{
		StorageObjectBuilderInterface* storageObjectBuilder_proxy = tp->createProxy( storageObjectBuilder);
		if (!storageObjectBuilder_proxy)
		{
			delete storageObjectBuilder;
			throw strus::runtime_error(_TXT("failed to create storage object builder trace proxy: %s"), errorhnd->fetchError());
		}
		storageObjectBuilder = storageObjectBuilder_proxy;
	}
	m_storage_objbuilder_impl.reset( storageObjectBuilder);
}

void ContextImpl::initAnalyzerObjBuilder()
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	AnalyzerObjectBuilderInterface* analyzerObjectBuilder = 0;
	if (m_rpc_impl.get())
	{
		RpcClientInterface* client = (RpcClientInterface*)m_rpc_impl.get();
		analyzerObjectBuilder = client->createAnalyzerObjectBuilder();
	}
	else if (m_moduleloader_impl.get())
	{
		ModuleLoaderInterface* moduleLoader = (ModuleLoaderInterface*)m_moduleloader_impl.get();
		analyzerObjectBuilder = moduleLoader->createAnalyzerObjectBuilder();
	}
	else
	{
		throw strus::runtime_error( _TXT("bad state, no context initialized"));
	}
	if (!analyzerObjectBuilder)
	{
		throw strus::runtime_error( _TXT("failed to create analyzer object builder: %s"), errorhnd->fetchError());
	}
	TraceProxy* tp = (TraceProxy*)m_trace_impl.get();
	if (tp)
	{
		AnalyzerObjectBuilderInterface* analyzerObjectBuilder_proxy = tp->createProxy( analyzerObjectBuilder);
		if (!analyzerObjectBuilder_proxy)
		{
			delete analyzerObjectBuilder;
			throw strus::runtime_error(_TXT("failed to create storage object builder trace proxy: %s"), errorhnd->fetchError());
		}
		analyzerObjectBuilder = analyzerObjectBuilder_proxy;
	}
	m_analyzer_objbuilder_impl.reset( analyzerObjectBuilder);
}

ValueVariant ContextImpl::detectDocumentClass( const std::string& content)
{
	if (!m_analyzer_objbuilder_impl.get()) initAnalyzerObjBuilder();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const AnalyzerObjectBuilderInterface* objBuilder = (const AnalyzerObjectBuilderInterface*)m_analyzer_objbuilder_impl.get();
	const TextProcessorInterface* textproc;
	if (m_textproc)
	{
		textproc = (const TextProcessorInterface*)m_textproc;
	}
	else
	{
		textproc = objBuilder->getTextProcessor();
		m_textproc = textproc;
		if (!textproc) throw strus::runtime_error( _TXT("failed to get text processor: %s"), errorhnd->fetchError());
	}
	analyzer::DocumentClass dclass;
	if (textproc->detectDocumentClass( dclass, content.c_str(), content.size()))
	{
		return DocumentClass( dclass.mimeType(), dclass.encoding(), dclass.scheme());
	}
	else
	{
		if (errorhnd->hasError()) throw strus::runtime_error( _TXT("failed to detect document class: %s"), errorhnd->fetchError());
		return DocumentClass();
	}
}

StorageClientImpl ContextImpl::createStorageClient( const std::string& config_)
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return StorageClient( m_storage_objbuilder_impl, m_trace_impl, m_errorhnd_impl, config_);
}

StorageClientImpl ContextImpl::createStorageClient()
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return StorageClient( m_storage_objbuilder_impl, m_trace_impl, m_errorhnd_impl, std::string());
}

VectorStorageClientImpl ContextImpl::createVectorStorageClient()
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return VectorStorageClient( m_storage_objbuilder_impl, m_trace_impl, m_errorhnd_impl, std::string());
}

VectorStorageClientImpl ContextImpl::createVectorStorageClient( const std::string& config_)
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return VectorStorageClient( m_storage_objbuilder_impl, m_trace_impl, m_errorhnd_impl, config_);
}

DocumentAnalyzerImpl ContextImpl::createDocumentAnalyzer( const std::string& segmentername_)
{
	if (!m_analyzer_objbuilder_impl.get()) initAnalyzerObjBuilder();
	if (!m_textproc)
	{
		AnalyzerObjectBuilderInterface* objBuilder = (AnalyzerObjectBuilderInterface*)m_analyzer_objbuilder_impl.get();
		m_textproc = objBuilder->getTextProcessor();
		if (!m_textproc)
		{
			ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
			throw strus::runtime_error( _TXT("failed to get text processor: %s"), errorhnd->fetchError());
		}
	}
	return DocumentAnalyzer( m_analyzer_objbuilder_impl, m_trace_impl, m_errorhnd_impl, segmentername_, m_textproc);
}

QueryAnalyzerImpl ContextImpl::createQueryAnalyzer()
{
	if (!m_analyzer_objbuilder_impl.get()) initAnalyzerObjBuilder();
	return QueryAnalyzer( m_analyzer_objbuilder_impl, m_trace_impl, m_errorhnd_impl);
}

QueryEvalImpl ContextImpl::createQueryEval()
{
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	return QueryEval( m_storage_objbuilder_impl, m_trace_impl, m_errorhnd_impl);
}

void ContextImpl::createStorage( const std::string& config_)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	std::string dbname;
	std::string storagecfg( config_);
	(void)extractStringFromConfigString( dbname, storagecfg, "database", errorhnd);

	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	StorageObjectBuilderInterface* objBuilder = (StorageObjectBuilderInterface*)m_storage_objbuilder_impl.get();
	const DatabaseInterface* dbi = objBuilder->getDatabase( dbname);
	if (!dbi) throw strus::runtime_error( _TXT("failed to get database: %s"), errorhnd->fetchError());
	const StorageInterface* sti = objBuilder->getStorage();
	if (!sti) throw strus::runtime_error( _TXT("failed to get storage: %s"), errorhnd->fetchError());
	if (!sti->createStorage( storagecfg, dbi)) throw strus::runtime_error( _TXT("failed to create storage: %s"), errorhnd->fetchError());
}

void ContextImpl::createVectorStorage( const std::string& config_)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	std::string dbname;
	std::string storagename;
	std::string storagecfg( config_);
	(void)extractStringFromConfigString( dbname, storagecfg, "database", errorhnd);
	if (!extractStringFromConfigString( dbname, storagename, "storage", errorhnd))
	{
		storagename = Constants::standard_vector_storage();
	}
	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	StorageObjectBuilderInterface* objBuilder = (StorageObjectBuilderInterface*)m_storage_objbuilder_impl.get();
	const DatabaseInterface* dbi = objBuilder->getDatabase( dbname);
	if (!dbi) throw strus::runtime_error( _TXT("failed to get database: %s"), errorhnd->fetchError());
	const VectorStorageInterface* sti = objBuilder->getVectorStorage( storagename);
	if (!sti) throw strus::runtime_error( _TXT("failed to get vector storage: %s"), errorhnd->fetchError());
	if (!sti->createStorage( storagecfg, dbi)) throw strus::runtime_error( _TXT("failed to create vector storage: %s"), errorhnd->fetchError());
}

void ContextImpl::destroyStorage( const std::string& config_)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	std::string dbname;
	std::string storagecfg( config_);
	(void)extractStringFromConfigString( dbname, storagecfg, "database", errorhnd);

	if (!m_storage_objbuilder_impl.get()) initStorageObjBuilder();
	StorageObjectBuilderInterface* objBuilder = (StorageObjectBuilderInterface*)m_storage_objbuilder_impl.get();
	if (!objBuilder) throw strus::runtime_error( _TXT("failed to get object builder: %s"), errorhnd->fetchError());
	const DatabaseInterface* dbi = objBuilder->getDatabase( dbname);
	if (!dbi) throw strus::runtime_error( _TXT("failed to get database: %s"), errorhnd->fetchError());
	if (!dbi->destroyDatabase( storagecfg)) throw strus::runtime_error( _TXT("failed to destroy database: %s"), errorhnd->fetchError());
}

void ContextImpl::close()
{
	m_analyzer_objbuilder_impl.reset();
	m_storage_objbuilder_impl.reset();
	if (m_rpc_impl.get()) ((RpcClientInterface*)m_rpc_impl.get())->close();
	m_moduleloader_impl.reset();
}


