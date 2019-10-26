/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/storage.hpp"
#include "impl/value/postingIterator.hpp"
#include "impl/value/valueIterator.hpp"
#include "impl/value/selectIterator.hpp"
#include "impl/value/statisticsIterator.hpp"
#include "impl/value/forwardTermsIterator.hpp"
#include "impl/value/searchTermsIterator.hpp"
#include "impl/value/storageIntrospection.hpp"
#include "strus/lib/storage_objbuild.hpp"
#include "strus/storageClientInterface.hpp"
#include "strus/storageTransactionInterface.hpp"
#include "strus/storageDocumentInterface.hpp"
#include "strus/storageClientInterface.hpp"
#include "strus/storageMetaDataTransactionInterface.hpp"
#include "strus/storageObjectBuilderInterface.hpp"
#include "strus/postingIteratorInterface.hpp"
#include "strus/attributeReaderInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/metaDataReaderInterface.hpp"
#include "strus/metaDataRestrictionInterface.hpp"
#include "strus/metaDataRestrictionInstanceInterface.hpp"
#include "strus/valueIteratorInterface.hpp"
#include "strus/forwardIteratorInterface.hpp"
#include "strus/base/configParser.hpp"
#include "strus/base/local_ptr.hpp"
#include "strus/base/string_format.hpp"
#include "papuga/serialization.h"
#include "serializer.hpp"
#include "valueVariantWrap.hpp"
#include "private/internationalization.hpp"
#include "expressionBuilder.hpp"
#include "deserializer.hpp"
#include "structDefs.hpp"
#include "serializer.hpp"
#include "callResultUtils.hpp"

using namespace strus;
using namespace strus::bindings;

StorageClientImpl::StorageClientImpl( const ObjectRef& trace, const ObjectRef& objbuilder, const ObjectRef& errorhnd_, const std::string& config_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl( trace)
	,m_objbuilder_impl( objbuilder)
	,m_storage_impl()
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StorageObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<const StorageObjectBuilderInterface>();

	m_storage_impl.resetOwnership( createStorageClient( objBuilder, errorhnd, config_), "StorageClient");
	if (!m_storage_impl.get())
	{
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
}

StorageClientImpl::~StorageClientImpl()
{}

void StorageClientImpl::reload( const ValueVariant& config_)
{
	std::string configstr = Deserializer::getConfigString( config_);
	StorageClientInterface* THIS = m_storage_impl.getObject<StorageClientInterface>();
	if (!THIS) throw strus::runtime_error( _TXT("calling storage client method after close"));
	if (!THIS->reload( configstr))
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( "error reloading configuration: %s", errorhnd->fetchError());
	}
}

static bool defineMetaDataTransactionOp( strus::StorageMetaDataTransactionInterface* transaction, ErrorBufferInterface* errorhnd, const PatchDef& patchdef)
{
	const char* context = "metadata";
	if (!patchdef.from.empty())
	{
		if (patchdef.path.size() != 2)
		{
			errorhnd->report( ErrorCodeNotImplemented, _TXT("element '%s' only allowed to have form '%s/<name>' in patch %s"),
						"from", context, context);
			return false;
		}
		if (patchdef.from[0] != context)
		{
			errorhnd->report( ErrorCodeNotImplemented, _TXT("element '%s' only allowed to have prefix '%s' in patch %s"),
						"from", context, context);
			return false;
		}
	}
	if (patchdef.path.size() > 2)
	{
		errorhnd->report( ErrorCodeNotFound, _TXT("element '%s' not valid for %s in patch %s"), "path", PatchDef::opName(patchdef.op), context);
		return false;
	}
	else if (patchdef.path[0] != context)
	{
		errorhnd->report( ErrorCodeNotImplemented, _TXT("element '%s' only allowed to have prefix '%s' in patch %s"), "path" , context, context);
		return false;
	}
	switch (patchdef.op)
	{
		case PatchDef::OpAdd:
			if (patchdef.value.empty())
			{
				errorhnd->report( ErrorCodeNotFound, _TXT("value mandatory for %s in %s"), PatchDef::opName(patchdef.op), context);
				return false;
			}
			else if (patchdef.path.size() == 2)
			{
				transaction->addElement( patchdef.path[1], patchdef.value);
			}
			else
			{
				errorhnd->report( ErrorCodeNotFound, _TXT("path not valid for %s in patch %s"), PatchDef::opName(patchdef.op), context);
				return false;
			}
			break;

		case PatchDef::OpRemove:
			if (!patchdef.value.empty())
			{
				errorhnd->report( ErrorCodeNotFound, _TXT("value not allowed for %s in %s"), PatchDef::opName(patchdef.op), context);
				return false;
			}
			else if (patchdef.path.size() == 1)
			{
				transaction->deleteElements();
			}
			else if (patchdef.path.size() == 2)
			{
				transaction->deleteElement( patchdef.path[1]);
			}
			break;

		case PatchDef::OpReplace:
			if (patchdef.path.size() == 2)
			{
				if (patchdef.value.empty())
				{
					if (patchdef.from.size() == 2)
					{
						transaction->renameElement( patchdef.from[1], patchdef.path[1]);
					}
					else
					{
						errorhnd->report( ErrorCodeIncompleteDefinition, "element 'from' missing (rename element) or 'value' missing (alter element)"); 
						return false;
					}
				}
				else if (patchdef.from.size() == 2)
				{
					transaction->alterElement( patchdef.from[1], patchdef.path[1], patchdef.value);
				}
				else
				{
					transaction->alterElement( patchdef.path[1], patchdef.path[1], patchdef.value);
				}
			}
			else
			{
				errorhnd->report( ErrorCodeNotFound, "path not valid for %s in patch %s", PatchDef::opName(patchdef.op), context);
				return false;
			}
			break;

		case PatchDef::OpCopy:
			if (patchdef.value != "0" || patchdef.value != "NULL")
			{
				errorhnd->report( ErrorCodeNotFound, "only 0 or NULL allowed as value for op '%s' in patch %s (clear content)", PatchDef::opName(patchdef.op), context);
				return false;
			}
			else if (patchdef.path.size() == 2)
			{
				transaction->clearElement( patchdef.path[1]);
			}
			else
			{
				errorhnd->report( ErrorCodeNotFound, "path not valid for %s in patch %s", PatchDef::opName(patchdef.op), context);
				return false;
			}
			break;

		case PatchDef::OpMove:
		case PatchDef::OpTest:
			errorhnd->report( ErrorCodeNotImplemented, "operator '%s' not implemented for patch %s", PatchDef::opName(patchdef.op), context);
			return false;
	}
	return true;
}

struct PatchConfigDef
{
	StorageClientInterface* m_cli;
	ErrorBufferInterface* m_errorhnd;
	std::set<std::string> m_validConfigSet;
	std::map<std::string,std::string> m_oldConfig;
	std::map<std::string,std::string> m_newConfig;

	PatchConfigDef( StorageClientInterface* cli_, ErrorBufferInterface* errorhnd_)
		:m_cli(cli_),m_errorhnd(errorhnd_),m_validConfigSet( getValidConfigSet(cli_)),m_oldConfig(),m_newConfig()
	{
		std::string oldStorageConfig = m_cli->config();
		std::set<std::string>::const_iterator ci = m_validConfigSet.begin(), ce = m_validConfigSet.end();
		for (; ci != ce; ++ci)
		{
			std::string value;
			if (strus::extractStringFromConfigString( value, oldStorageConfig, ci->c_str(), m_errorhnd))
			{
				m_oldConfig[ *ci] = value;
			}
		}
		if (!oldStorageConfig.empty())
		{
			m_errorhnd->report( ErrorCodeLogicError, _TXT("internal: inconsistency in storage config description: %s"), oldStorageConfig.c_str());
		}
	}

	bool isValidKey( std::string& key) const
	{
		return m_validConfigSet.find( key) != m_validConfigSet.end();
	}

	void add( const std::string& key, const std::string& value)
	{
		m_oldConfig.erase( key);
		m_newConfig[ key] = value;
	}

	void remove( const std::string& key)
	{
		m_oldConfig.erase( key);
		m_newConfig.erase( key);
	}

	static std::set<std::string> getValidConfigSet( StorageClientInterface* cli_)
	{
		std::set<std::string> rt;
		char const** ai = cli_->getConfigParameters();
		for (; *ai; ++ai)
		{
			rt.insert( *ai);
		}
		return rt;
	}

	std::string tostring()
	{
		std::string rt;
		std::map<std::string,std::string>::const_iterator ci = m_oldConfig.begin(), ce = m_oldConfig.end();
		for (; ci != ce; ++ci)
		{
			if (strus::addConfigStringItem( rt, ci->first, ci->second, m_errorhnd)) return std::string();
		}
		ci = m_newConfig.begin(), ce = m_newConfig.end();
		for (; ci != ce; ++ci)
		{
			if (strus::addConfigStringItem( rt, ci->first, ci->second, m_errorhnd)) return std::string();
		}
		return rt;
	}
};

static bool defineConfigPatchOp( PatchConfigDef& cfg, ErrorBufferInterface* errorhnd, const PatchDef& patchdef)
{
	const char* context = "storage configuration";
	if (!patchdef.from.empty())
	{
		errorhnd->report( ErrorCodeNotImplemented, "element 'from' not implemented in patch %s" , context);
		return false;
	}
	if (!patchdef.path.empty())
	{
		errorhnd->report( ErrorCodeNotImplemented, "element 'path' required in every element of patch %s" , context);
		return false;
	}
	if (patchdef.path.size() > 1)
	{
		errorhnd->report( ErrorCodeNotFound, "path with more than one identifier not allowed in patch %s", context);
		return false;
	}
	switch (patchdef.op)
	{
		case PatchDef::OpAdd:
			if (patchdef.value.empty())
			{
				errorhnd->report( ErrorCodeNotFound, _TXT("value mandatory for %s in %s"), PatchDef::opName(patchdef.op), context);
				return false;
			}
			cfg.add( patchdef.path[0], patchdef.value);
			break;
		case PatchDef::OpRemove:
			if (!patchdef.value.empty())
			{
				errorhnd->report( ErrorCodeNotFound, _TXT("value not allowed for %s in %s"), PatchDef::opName(patchdef.op), context);
				return false;
			}
			cfg.remove( patchdef.path[0]);
			break;
		case PatchDef::OpReplace:
		case PatchDef::OpCopy:
		case PatchDef::OpMove:
		case PatchDef::OpTest:
			errorhnd->report( ErrorCodeNotImplemented, _TXT("operator '%s' not implemented for %s"), PatchDef::opName(patchdef.op), context);
			return false;
	}
	return true;
}

void StorageClientImpl::patch( const ValueVariant& patchlist)
{
	StorageClientInterface* THIS = m_storage_impl.getObject<StorageClientInterface>();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (!THIS) throw strus::runtime_error( _TXT("calling storage client method after close"));

	std::vector<PatchDef> patchDefList = PatchDef::parseList( patchlist);
	std::vector<PatchDef>::iterator pi = patchDefList.begin(), pe = patchDefList.end();
	strus::Reference<strus::StorageMetaDataTransactionInterface> transaction( THIS->createMetaDataTransaction());
	PatchConfigDef configDef( THIS, errorhnd);

	for (; pi != pe; ++pi)
	{
		if (pi->path.empty())
		{
			errorhnd->report( ErrorCodeNotFound, _TXT("path has to be defined always in a %s"), "patch definition list");
			break;
		}
		if (pi->path[0] == "metadata")
		{
			if (!defineMetaDataTransactionOp( transaction.get(), errorhnd, *pi)) break;
		}
		else if (configDef.isValidKey( pi->path[0]))
		{
			if (!defineConfigPatchOp( configDef, errorhnd, *pi)) break;
		}
		else
		{
			errorhnd->report( ErrorCodeNotImplemented, _TXT("unknown configuration parameter '%s' (case sensitive)"), pi->path[0].c_str());
			break;
		}
	}

	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT("failed to patch storage configuration: %s"), errorhnd->fetchError());
	}
	if (!transaction->commit())
	{
		throw strus::runtime_error(_TXT("meta data transaction failed when patching storage configuration: %s"), errorhnd->fetchError());
	}
	strus::Reference<MetaDataReaderInterface> metadataReader( THIS->createMetaDataReader());
	if (!metadataReader.get())
	{
		throw strus::runtime_error(_TXT("failed to create meta data reader: %s"), errorhnd->fetchError());
	}
	std::string metadatadef;
	strus::Index ei = 0, ee = metadataReader->nofElements();
	for (; ei != ee; ++ei)
	{
		if (!metadatadef.empty()) metadatadef.push_back(',');
		metadatadef.append( strus::string_format("%s %s", metadataReader->getName(ei), metadataReader->getType(ei)));
	}
	configDef.add( "metadata", metadatadef);

	std::string configstr = configDef.tostring();
	if (!THIS->reload( configstr))
	{
		throw strus::runtime_error( "error reloading configuration: %s", errorhnd->fetchError());
	}
}

long StorageClientImpl::nofDocumentsInserted() const
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

Index StorageClientImpl::documentNumber( const std::string& docid_) const
{
	const StorageClientInterface* THIS = m_storage_impl.getObject<const StorageClientInterface>();
	if (!THIS) throw strus::runtime_error( _TXT("calling storage client method after close"));
	return THIS->documentNumber( docid_);
}

Iterator StorageClientImpl::documentForwardIndexTerms( const Index& docno, const std::string& termtype, const Index& pos) const
{
	Reference<ForwardTermsIterator> itr( new ForwardTermsIterator( m_trace_impl, m_objbuilder_impl, m_storage_impl, m_errorhnd_impl, termtype, docno, pos));
	Iterator rt( itr.get(), &ForwardTermsIterator::Deleter, &ForwardTermsIterator::GetNext);
	itr.release();
	rt.release();
	return rt;
}

Iterator StorageClientImpl::documentSearchIndexTerms( const Index& docno, const std::string& termtype) const
{
	Reference<SearchTermsIterator> itr( new SearchTermsIterator( m_trace_impl, m_objbuilder_impl, m_storage_impl, m_errorhnd_impl, termtype, docno));
	Iterator rt( itr.get(), &SearchTermsIterator::Deleter, &SearchTermsIterator::GetNext);
	itr.release();
	rt.release();
	return rt;
}

Iterator StorageClientImpl::postings( const ValueVariant& expression, const ValueVariant& restriction, const Index& start_docno)
{
	Reference<PostingIterator> itr( new PostingIterator( m_trace_impl, m_objbuilder_impl, m_storage_impl, m_errorhnd_impl, expression, restriction, start_docno));
	Iterator rt( itr.get(), &PostingIterator::Deleter, &PostingIterator::GetNext);
	itr.release();
	rt.release();
	return rt;
}

Iterator StorageClientImpl::select( const ValueVariant& what, const ValueVariant& expression, const ValueVariant& restriction, const Index& start_docno, const ValueVariant& accesslist)
{
	Reference<SelectIterator> itr( new SelectIterator( m_trace_impl, m_objbuilder_impl, m_storage_impl, m_errorhnd_impl, what, expression, restriction, start_docno, accesslist));
	Iterator rt( itr.get(), &SelectIterator::Deleter, &SelectIterator::GetNext);
	itr.release();
	rt.release();
	return rt;
}

Iterator StorageClientImpl::termTypes() const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling storage client method after close"));
	ObjectRef itr( ObjectRef::createOwnership( storage->createTermTypeIterator(), "TermTypeIterator"));
	if (!itr.get()) throw strus::runtime_error("%s", errorhnd->fetchError());
	Iterator rt( new ValueIterator( m_trace_impl, m_objbuilder_impl, m_storage_impl, itr, m_errorhnd_impl), &ValueIterator::Deleter, &ValueIterator::GetNext);
	rt.release();
	return rt;
}

Iterator StorageClientImpl::termValues() const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling storage client method after close"));
	ObjectRef itr( ObjectRef::createOwnership( storage->createTermValueIterator(), "TermTypeIterator"));
	if (!itr.get()) throw strus::runtime_error("%s", errorhnd->fetchError());
	Iterator rt( new ValueIterator( m_trace_impl, m_objbuilder_impl, m_storage_impl, itr, m_errorhnd_impl), &ValueIterator::Deleter, &ValueIterator::GetNext);
	rt.release();
	return rt;
}

Iterator StorageClientImpl::docids() const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling storage client method after close"));
	ObjectRef itr( ObjectRef::createOwnership( storage->createDocIdIterator(), "DocIdIterator"));
	if (!itr.get()) throw strus::runtime_error( "%s", errorhnd->fetchError());
	Iterator rt( new ValueIterator( m_trace_impl, m_objbuilder_impl, m_storage_impl, itr, m_errorhnd_impl), &ValueIterator::Deleter, &ValueIterator::GetNext);
	rt.release();
	return rt;
}

std::string StorageClientImpl::docid( const Index& docno) const
{
	const StorageClientInterface* THIS = m_storage_impl.getObject<const StorageClientInterface>();
	if (!THIS) throw strus::runtime_error( _TXT("calling storage client method after close"));
	Reference<AttributeReaderInterface> areader( THIS->createAttributeReader());
	if (!areader.get())
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
	Index eh = areader->elementHandle( Constants::attribute_docid());
	if (!eh) throw strus::runtime_error( _TXT("attribute '%s' not defined"), Constants::attribute_docid());
	areader->skipDoc( docno);
	return areader->getValue( eh);
}

Iterator StorageClientImpl::usernames() const
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling storage client method after close"));
	ObjectRef itr( ObjectRef::createOwnership( storage->createUserNameIterator(), "UserNameIterator"));
	if (!itr.get()) throw strus::runtime_error( "%s", errorhnd->fetchError());
	Iterator rt( new ValueIterator( m_trace_impl, m_objbuilder_impl, m_storage_impl, itr, m_errorhnd_impl), &ValueIterator::Deleter, &ValueIterator::GetNext);
	rt.release();
	return rt;
}

std::vector<std::string>* StorageClientImpl::attributeNames() const
{
	const StorageClientInterface* storage = m_storage_impl.getObject<const StorageClientInterface>();
	Reference<AttributeReaderInterface> reader( storage->createAttributeReader());
	if (!reader.get())
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
	return new std::vector<std::string>( reader->getNames());
}

std::vector<std::string>* StorageClientImpl::metadataNames() const
{
	const StorageClientInterface* storage = m_storage_impl.getObject<const StorageClientInterface>();
	Reference<MetaDataReaderInterface> reader( storage->createMetaDataReader());
	if (!reader.get())
	{
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
	return new std::vector<std::string>( reader->getNames());
}

Iterator StorageClientImpl::getAllStatistics()
{
	Iterator rt( new StatisticsIterator( m_trace_impl, m_objbuilder_impl, m_storage_impl, m_errorhnd_impl), &StatisticsIterator::Deleter, &StatisticsIterator::GetNext);
	rt.release();
	return rt;
}

Iterator StorageClientImpl::getChangeStatistics( const ValueVariant& timestamp)
{
	Iterator rt( new StatisticsIterator( m_trace_impl, m_objbuilder_impl, m_storage_impl, m_errorhnd_impl, timestamp), &StatisticsIterator::Deleter, &StatisticsIterator::GetNext);
	rt.release();
	return rt;
}

StorageTransactionImpl* StorageClientImpl::createTransaction() const
{
	if (!m_storage_impl.get()) throw strus::runtime_error( _TXT("calling storage client method after close"));
	return new StorageTransactionImpl( m_trace_impl, m_objbuilder_impl, m_errorhnd_impl, m_storage_impl);
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

void StorageClientImpl::compaction()
{
	if (!m_storage_impl.get()) throw strus::runtime_error( _TXT("calling storage client method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	bool preverr = errorhnd->hasError();
	StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	storage->compaction();
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

Struct StorageClientImpl::config() const
{
	const StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	if (!storage) throw strus::runtime_error( _TXT("calling storage client method after close"));
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();

	ConfigStruct rt( storage->config(), errorhnd);
	rt.release();
	return rt;
}

Struct StorageClientImpl::introspection( const ValueVariant& arg) const
{
	Struct rt;
	std::vector<std::string> path;
	if (papuga_ValueVariant_defined( &arg))
	{
		path = Deserializer::getStringList( arg);
	}
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	const StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();

	strus::local_ptr<IntrospectionBase> ictx( new StorageIntrospection( errorhnd, storage));
	ictx->getPathContent( rt.serialization, path, false/*substructure*/);
	if (errorhnd->hasError())
	{
		throw strus::runtime_error(_TXT( "failed to serialize introspection: %s"), errorhnd->fetchError());
	}
	rt.release();
	return rt;
}

StorageTransactionImpl::StorageTransactionImpl( const ObjectRef& trace_, const ObjectRef& objbuilder_, const ObjectRef& errorhnd_, const ObjectRef& storage_)
	:m_errorhnd_impl(errorhnd_)
	,m_trace_impl(trace_)
	,m_objbuilder_impl(objbuilder_)
	,m_storage_impl(storage_)
	,m_transaction_impl()
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	StorageClientInterface* storage = m_storage_impl.getObject<StorageClientInterface>();
	StorageTransactionInterface* transaction = storage->createTransaction();
	if (!transaction) throw strus::runtime_error( "%s", errorhnd->fetchError());
	m_transaction_impl.resetOwnership( transaction, "StorageTransaction");
}

void StorageTransactionImpl::insertDocument( const std::string& docid, const ValueVariant& doc)
{
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	StorageTransactionInterface* transaction = m_transaction_impl.getObject<StorageTransactionInterface>();
	if (!transaction) throw strus::runtime_error( _TXT("try to insert document in a closed transaction"));
	Reference<StorageDocumentInterface> document( transaction->createDocument( docid));
	if (!document.get()) throw strus::runtime_error( _TXT("failed to create document with id '%s' to insert: %s"), docid.c_str(), errorhnd->fetchError());

	Deserializer::buildInsertDocument( document.get(), doc, errorhnd);
	document->done();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("failed to insert document: %s"), errorhnd->fetchError());
	}
}

void StorageTransactionImpl::deleteDocument( const std::string& docId)
{
	StorageTransactionInterface* transaction = m_transaction_impl.getObject<StorageTransactionInterface>();
	if (!transaction) throw strus::runtime_error( _TXT("try to delete document in a closed transaction"));
	transaction->deleteDocument( docId);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("failed to insert document: %s"), errorhnd->fetchError());
	}
}

void StorageTransactionImpl::deleteUserAccessRights( const std::string& username)
{
	StorageTransactionInterface* transaction = m_transaction_impl.getObject<StorageTransactionInterface>();
	if (!transaction) throw strus::runtime_error( _TXT("try to delete user access rights in a closed transaction"));
	transaction->deleteUserAccessRights( username);
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("failed to insert document: %s"), errorhnd->fetchError());
	}
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



