/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/storage.hpp"
#include "impl/statistics.hpp"
#include "impl/struct.hpp"
#include "strus/lib/storage_objbuild.hpp"
#include "strus/storageClientInterface.hpp"
#include "strus/storageTransactionInterface.hpp"
#include "strus/storageDocumentInterface.hpp"
#include "strus/storageClientInterface.hpp"
#include "strus/storageObjectBuilderInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/postingIteratorInterface.hpp"
#include "strus/attributeReaderInterface.hpp"
#include "strus/statisticsIteratorInterface.hpp"
#include "strus/metaDataReaderInterface.hpp"
#include "strus/metaDataRestrictionInterface.hpp"
#include "strus/metaDataRestrictionInstanceInterface.hpp"
#include "strus/valueIteratorInterface.hpp"
#include "strus/base/configParser.hpp"
#include "papuga/serialization.hpp"
#include "serializer.hpp"
#include "valueVariantWrap.hpp"
#include "internationalization.hpp"
#include "metadataop.hpp"
#include "expressionBuilder.hpp"
#include "deserializer.hpp"
#include "serializer.hpp"
#include "callResultUtils.hpp"

using namespace strus;
using namespace strus::bindings;

StorageClientImpl::StorageClientImpl( const ObjectRef& objbuilder, const ObjectRef& trace, const ObjectRef& errorhnd_, const std::string& config_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl( trace)
	,m_objbuilder_impl( objbuilder)
	,m_storage_impl()
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StorageObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<const StorageObjectBuilderInterface>();

	m_storage_impl.resetOwnership( createStorageClient( objBuilder, errorhnd, config_));
	if (!m_storage_impl.get())
	{
		throw strus::runtime_error( _TXT("failed to create storage client: %s"), errorhnd->fetchError());
	}
}

unsigned int StorageClientImpl::nofDocumentsInserted() const
{
	const StorageClientInterface* THIS = m_storage_impl.getObject<const StorageClientInterface>();
	if (!THIS) throw strus::runtime_error( _TXT("calling storage client method after close"));
	return THIS->nofDocumentsInserted();
}

Index StorageClientImpl::documentFrequency( const std::string& type, const std::string& term) const
{
	const StorageClientInterface* THIS = m_storage_impl.getObject<const StorageClientInterface>();
	if (!THIS) throw strus::runtime_error( _TXT("calling storage client method after close"));
	return THIS->documentFrequency( type, term);
}

Index StorageClientImpl::documentNumber( const std::string& docid) const
{
	const StorageClientInterface* THIS = m_storage_impl.getObject<const StorageClientInterface>();
	if (!THIS) throw strus::runtime_error( _TXT("calling storage client method after close"));
	return THIS->documentNumber( docid);
}

class PostingIterator
{
public:
	PostingIterator( const ObjectRef& objbuilder_, const ObjectRef& storage_, const ObjectRef& errorhnd_, const ValueVariant& expression, const ValueVariant& restriction, const Index& docno_)
		:m_objbuilder_impl(objbuilder_),m_storage_impl(storage_),m_errorhnd_impl(errorhnd_),m_postings(),m_restriction(),m_docno(docno_?docno_:1)
	{
		const StorageObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<const StorageObjectBuilderInterface>();
		const StorageClientInterface* storage = m_storage_impl.getObject<const StorageClientInterface>();
		const QueryProcessorInterface* queryproc = objBuilder->getQueryProcessor();
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		if (!storage) throw strus::runtime_error( _TXT("calling storage client method after close"));
		PostingsExpressionBuilder postingsBuilder( storage, queryproc, errorhnd);
		Deserializer::buildExpression( postingsBuilder, expression, errorhnd);
		m_postings = postingsBuilder.pop();
		if (papuga_ValueVariant_defined( &restriction))
		{
			Reference<MetaDataRestrictionInterface> builder;
			builder.reset( storage->createMetaDataRestriction());
			if (!builder.get()) throw strus::runtime_error(_TXT("failed to create metadata restriction for posting iterator"));
			Deserializer::buildMetaDataRestriction( builder.get(), restriction, errorhnd);
			m_restriction.reset( builder->createInstance());
			if (!m_restriction.get()) throw strus::runtime_error(_TXT("failed to create metadata restriction for posting iterator instance"));
		}
	}
	~PostingIterator(){}

	std::pair<Index,std::vector<Index> > getNext()
	{
		std::pair<Index,std::vector<Index> > rt( 0, std::vector<Index>());
		if (!m_docno) return rt;
		for (; 0!=(m_docno = m_postings->skipDoc( m_docno)); ++m_docno)
		{
			if (!m_restriction.get() || m_restriction->match( m_docno)) break;
		}
		if (m_docno)
		{
			rt.first = m_docno++;
			for (Index pos = 0; 0!=(pos=m_postings->skipPos(pos)); ++pos)
			{
				rt.second.push_back( pos);
			}
		}
		else
		{
			ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
			if (errorhnd->hasError()) throw strus::runtime_error(_TXT("error in posting iterator: %s"), errorhnd->fetchError());
		}
		return rt;
	}

private:
	ObjectRef m_objbuilder_impl;
	ObjectRef m_storage_impl;
	ObjectRef m_errorhnd_impl;
	Reference<PostingIteratorInterface> m_postings;
	Reference<MetaDataRestrictionInstanceInterface> m_restriction;
	Index m_docno;
};

static bool PostingsGetNext( void* self, papuga_CallResult* result)
{
	try
	{
		std::pair<Index,std::vector<Index> > res = ((PostingIterator*)self)->getNext();
		if (!res.first) return false;

		bool ser = true;
		papuga_set_CallResult_serialization( result);
		ser &= papuga_Serialization_pushOpen( &result->serialization);
		ser &= papuga_Serialization_pushValue_int( &result->serialization, res.first);
		ser &= Serializer::serializeIntArray( &result->serialization, res.second);
		ser &= papuga_Serialization_pushClose( &result->serialization);
		if (!ser)
		{
			papuga_CallResult_reportError( result, _TXT("memory allocation error in postings iterator get next"));
			return false;
		}
		return true;
	}
	catch (const std::bad_alloc& err)
	{
		papuga_CallResult_reportError( result, _TXT("memory allocation error in postings iterator get next"));
		return false;
	}
	catch (const std::runtime_error& err)
	{
		papuga_CallResult_reportError( result, _TXT("error in postings iterator get next: %s"), err.what());
		return false;
	}
}

static void PostingsDeleter( void* obj)
{
	delete (PostingIterator*)obj;
}

class ValueIterator
{
public:
	enum {MaxNofElements=128};

	ValueIterator( const Reference<ValueIteratorInterface>& values_, const ObjectRef& errorhnd_, const std::string& key=std::string())
		:m_values(values_),m_errorhnd_impl(errorhnd_)
	{
		if (!key.empty())
		{
			m_values->skip( key.c_str(), key.size());
		}
		m_block = m_values->fetchValues( MaxNofElements);
		m_blockitr = m_block.begin();
	}
	~ValueIterator(){}

	bool getNext( papuga_CallResult* result)
	{
		if (m_blockitr == m_block.end())
		{
			if (m_block.empty()) return false;
			m_block = m_values->fetchValues( MaxNofElements);
			m_blockitr = m_block.begin();
			if (m_blockitr == m_block.end())
			{
				ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
				if (errorhnd->hasError()) throw strus::runtime_error(_TXT("error in posting iterator: %s"), errorhnd->fetchError());
				return false;
			}
		}
		if (!papuga_set_CallResult_string( result, m_blockitr->c_str(), m_blockitr->size())) throw std::bad_alloc();
		++m_blockitr;
		return true;
	}

private:
	Reference<ValueIteratorInterface> m_values;
	ObjectRef m_errorhnd_impl;
	std::vector<std::string> m_block;
	std::vector<std::string>::const_iterator m_blockitr;
};


static bool ValueIteratorGetNext( void* self, papuga_CallResult* result)
{
	try
	{
		return (((ValueIterator*)self)->getNext( result));
	}
	catch (const std::bad_alloc& err)
	{
		papuga_CallResult_reportError( result, _TXT("memory allocation error in value iterator get next"));
		return false;
	}
	catch (const std::runtime_error& err)
	{
		papuga_CallResult_reportError( result, _TXT("error in value iterator get next: %s"), err.what());
		return false;
	}
}

static void ValueIteratorDeleter( void* obj)
{
	delete (ValueIterator*)obj;
}

Iterator StorageClientImpl::postings( const ValueVariant& expression, const ValueVariant& restriction, const Index& start_docno)
{
	Reference<PostingIterator> itr( new PostingIterator( m_objbuilder_impl, m_storage_impl, m_errorhnd_impl, expression, restriction, start_docno));
	Iterator rt( itr.get(), &PostingsDeleter, &PostingsGetNext);
	itr.release();
	return rt;
}

Iterator StorageClientImpl::termTypes() const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling storage client method after close"));
	Reference<ValueIteratorInterface> itr( storage->createTermTypeIterator());
	if (!itr.get()) throw strus::runtime_error(_TXT("error creating term type iterator: %s"), errorhnd->fetchError());
	Iterator rt( new ValueIterator( itr, m_errorhnd_impl), &ValueIteratorDeleter, &ValueIteratorGetNext);
	rt.release();
	return rt;
}

StorageTransactionImpl* StorageClientImpl::createTransaction() const
{
	if (!m_storage_impl.get()) throw strus::runtime_error( _TXT("calling storage client method after close"));
	return new StorageTransactionImpl( m_objbuilder_impl, m_trace_impl, m_errorhnd_impl, m_storage_impl);
}

StatisticsIteratorImpl* StorageClientImpl::createStatisticsIterator( bool sign)
{
	StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling storage client method after close"));
	ObjectRef iter;
	iter.resetOwnership( storage->createStatisticsIterator( sign));
	if (!iter.get())
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( _TXT("failed to create statistics iterator: %s"), errorhnd->fetchError());
	}
	return new StatisticsIteratorImpl( m_objbuilder_impl, m_trace_impl, m_errorhnd_impl, m_storage_impl, iter);
}

StatisticsIteratorImpl* StorageClientImpl::createUpdateStatisticsIterator()
{
	StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling storage client method after close"));
	ObjectRef iter;
	iter.resetOwnership( storage->createUpdateStatisticsIterator());
	if (!iter.get())
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( _TXT("failed to create statistics iterator: %s"), errorhnd->fetchError());
	}
	return new StatisticsIteratorImpl( m_objbuilder_impl, m_trace_impl, m_errorhnd_impl, m_storage_impl, iter);
}

DocumentBrowserImpl* StorageClientImpl::createDocumentBrowser() const
{
	if (!m_objbuilder_impl.get()) throw strus::runtime_error( _TXT("calling storage client method after close"));
	return new DocumentBrowserImpl( m_objbuilder_impl, m_trace_impl, m_storage_impl, m_errorhnd_impl);
}

void StorageClientImpl::close()
{
	if (!m_storage_impl.get()) throw strus::runtime_error( _TXT("calling storage client method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	bool preverr = errorhnd->hasError();
	StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	storage->close();
	m_storage_impl.reset();
	if (!preverr && errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("error detected after calling storage client close: %s"), errorhnd->fetchError());
	}
}

std::string StorageClientImpl::configstring() const
{
	const StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling storage client method after close"));
	return storage->config();
}

std::vector<std::pair<std::string,std::string> >* StorageClientImpl::config() const
{
	const StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling storage client method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();

	typedef std::vector<std::pair<std::string,std::string> > Configuration;
	Configuration* rt( new Configuration( getConfigStringItems( storage->config(), errorhnd)));
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to get the storage configuration: %s"), errorhnd->fetchError());
	}
	return rt;
}

StorageTransactionImpl::StorageTransactionImpl( const ObjectRef& objbuilder_, const ObjectRef& trace_, const ObjectRef& errorhnd_, const ObjectRef& storage_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl(trace_)
	,m_objbuilder_impl(objbuilder_)
	,m_storage_impl(storage_)
	,m_transaction_impl()
{}

StorageTransactionInterface* StorageTransactionImpl::getTransaction()
{
	StorageTransactionInterface* rt;
	if (!m_transaction_impl.get())
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
		m_transaction_impl.resetOwnership( rt = storage->createTransaction());
		if (!rt) throw strus::runtime_error( _TXT("failed to create transaction: %s"), errorhnd->fetchError());
	}
	else
	{
		rt = m_transaction_impl.getObject<StorageTransactionInterface>();
	}
	return rt;
}

void StorageTransactionImpl::insertDocument( const std::string& docid, const ValueVariant& doc)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	StorageTransactionInterface* transaction = getTransaction();
	Reference<StorageDocumentInterface> document( transaction->createDocument( docid));
	if (!document.get()) throw strus::runtime_error( _TXT("failed to create document with id '%s' to insert: %s"), docid.c_str(), errorhnd->fetchError());

	Deserializer::buildInsertDocument( document.get(), doc, errorhnd);
	document->done();
}

void StorageTransactionImpl::deleteDocument( const std::string& docId)
{
	StorageTransactionInterface* transaction = getTransaction();
	transaction->deleteDocument( docId);
}

void StorageTransactionImpl::deleteUserAccessRights( const std::string& username)
{
	StorageTransactionInterface* transaction = getTransaction();
	transaction->deleteUserAccessRights( username);
}

void StorageTransactionImpl::commit()
{
	StorageTransactionInterface* transaction = m_transaction_impl.getObject<StorageTransactionInterface>();
	if (transaction)
	{
		if (!transaction->commit())
		{
			m_transaction_impl.reset();
			ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
			throw strus::runtime_error( _TXT("error flushing storage operations: %s"), errorhnd->fetchError());
		}
		m_transaction_impl.reset();
	}
}

void StorageTransactionImpl::rollback()
{
	m_transaction_impl.reset();
}

DocumentBrowserImpl::DocumentBrowserImpl( const ObjectRef& objbuilder_impl_, const ObjectRef& trace_impl_, const ObjectRef& storage_impl_, const ObjectRef& errorhnd_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl(trace_impl_)
	,m_objbuilder_impl(objbuilder_impl_)
	,m_storage_impl(storage_impl_)
	,m_restriction_impl()
	,m_postingitr_impl()
	,m_attributereader_impl()
	,m_metadatareader_impl()
{
	const StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	m_restriction_impl.resetOwnership( storage->createMetaDataRestriction());
	if (!m_restriction_impl.get())
	{
		throw strus::runtime_error( _TXT("failed to create meta data restriction interface for browsing documents"));
	}
	m_attributereader_impl.resetOwnership( storage->createAttributeReader());
	m_metadatareader_impl.resetOwnership( storage->createMetaDataReader());
}

void DocumentBrowserImpl::addMetaDataRestrictionCondition(
		const char* compareOp, const std::string& name,
		const ValueVariant& value, bool newGroup)
{
	MetaDataRestrictionInterface* restriction = m_restriction_impl.getObject<MetaDataRestrictionInterface>();
	if (!restriction)
	{
		throw strus::runtime_error( _TXT("it is not allowed to add more restrictions to a document browser after the first call of next()"));
	}
	MetaDataRestrictionInterface::CompareOperator cmpop = MetaDataOp::getCompareOp( compareOp);
	restriction->addCondition( cmpop, name, ValueVariantWrap::tonumeric(value), newGroup);
}

Index DocumentBrowserImpl::skipDoc( int docno)
{
	if (!m_postingitr_impl.get())
	{
		const StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
		MetaDataRestrictionInterface* restriction = m_restriction_impl.getObject<MetaDataRestrictionInterface>();
		m_postingitr_impl.resetOwnership( storage->createBrowsePostingIterator( restriction, 1));
		if (!m_postingitr_impl.get())
		{
			throw strus::runtime_error( _TXT("failed to create posting iterator for document browser"));
		}
	}
	PostingIteratorInterface* itr = m_postingitr_impl.getObject<PostingIteratorInterface>();
	return itr->skipDoc( docno);
}

Struct DocumentBrowserImpl::get( int docno, const ValueVariant& elementsSelected)
{
	if (docno <= 0) throw strus::runtime_error(_TXT("document browser get called without valid document selected"));

	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	std::vector<std::string> elemlist = Deserializer::getStringList( elementsSelected);
	AttributeReaderInterface* attributereader = m_attributereader_impl.getObject<AttributeReaderInterface>();
	MetaDataReaderInterface* metadatareader = m_metadatareader_impl.getObject<MetaDataReaderInterface>();
	bool attributereader_called = false;
	bool metadatareader_called = false;

	Struct rt;
	papuga_Serialization* res = &rt.serialization;
	bool serflag = true;
	serflag &= papuga_Serialization_pushOpen( res);

	std::vector<std::string>::const_iterator ei = elemlist.begin(), ee = elemlist.end();
	for (int eidx=0; ei != ee; ++ei,++eidx)
	{
		Index eh;
		if (0 <= (eh = metadatareader->elementHandle( ei->c_str())))
		{
			if (!metadatareader_called)
			{
				metadatareader->skipDoc( docno);
				metadatareader_called = true;
			}
			serflag &= Serializer::serialize_nothrow( res, metadatareader->getValue( eh));
		}
		else if (0 != (eh = attributereader->elementHandle( ei->c_str())))
		{
			if (!attributereader_called)
			{
				attributereader->skipDoc( docno);
				attributereader_called = true;
			}
			std::string attrvalstr = attributereader->getValue( eh);
			const char* attrvalptr = papuga_Allocator_copy_string( &rt.allocator, attrvalstr.c_str(), attrvalstr.size());
			if (!attrvalptr) throw std::bad_alloc();

			serflag &= papuga_Serialization_pushValue_string( res, attrvalptr, attrvalstr.size());
		}
		else
		{
			serflag &= papuga_Serialization_pushValue_void( res);
		}
	}
	serflag &= papuga_Serialization_pushClose( res);
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("error getting document attributes and metadata with document browser: %s"), errorhnd->fetchError());
	}
	if (!serflag) throw std::bad_alloc();
	rt.release();
	return rt;
}


