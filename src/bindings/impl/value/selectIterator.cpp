/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "impl/value/selectIterator.hpp"
#include "papuga/valueVariant.h"
#include "papuga/callResult.h"
#include "papuga/allocator.h"
#include "strus/errorBufferInterface.hpp"
#include "strus/storageObjectBuilderInterface.hpp"
#include "strus/storageClientInterface.hpp"
#include "strus/metaDataReaderInterface.hpp"
#include "strus/attributeReaderInterface.hpp"
#include "internationalization.hpp"
#include "expressionBuilder.hpp"
#include "deserializer.hpp"
#include "serializer.hpp"

#define ITERATOR_NAME "select iterator"

using namespace strus;
using namespace strus::bindings;

SelectIterator::SelectIterator( const ObjectRef& trace_, const ObjectRef& objbuilder_, const ObjectRef& storage_, const ObjectRef& errorhnd_, const papuga_ValueVariant& what, const papuga_ValueVariant& expression, const papuga_ValueVariant& restriction, const Index& start_docno_)
	:m_trace_impl(trace_),m_objbuilder_impl(objbuilder_),m_storage_impl(storage_),m_attributereader_impl(),m_metadatareader_impl(),m_errorhnd_impl(errorhnd_),m_postings(),m_restriction(),m_forwarditer(),m_docno(start_docno_?start_docno_:1),m_maxdocno(0),m_items()
{
	const StorageObjectBuilderInterface* objBuilder = m_objbuilder_impl.getObject<const StorageObjectBuilderInterface>();
	const StorageClientInterface* storage = m_storage_impl.getObject<const StorageClientInterface>();
	const QueryProcessorInterface* queryproc = objBuilder->getQueryProcessor();
	ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();

	m_attributereader_impl.resetOwnership( storage->createAttributeReader(), "AttributeReader");
	if (!m_attributereader_impl.get()) throw strus::runtime_error( _TXT("failed to create attribute reader for %s"), ITERATOR_NAME);
	m_metadatareader_impl.resetOwnership( storage->createMetaDataReader(), "MetaDataReader");
	if (!m_metadatareader_impl.get()) throw strus::runtime_error( _TXT("failed to create metadata reader for %s"), ITERATOR_NAME);

	AttributeReaderInterface* attributereader = m_attributereader_impl.getObject<AttributeReaderInterface>();
	MetaDataReaderInterface* metadatareader = m_metadatareader_impl.getObject<MetaDataReaderInterface>();
	m_maxdocno = storage->maxDocumentNumber();

	if (papuga_ValueVariant_defined( &expression))
	{
		PostingsExpressionBuilder postingsBuilder( storage, queryproc, errorhnd);
		Deserializer::buildExpression( postingsBuilder, expression, errorhnd);
		m_postings = postingsBuilder.pop();
	}
	if (papuga_ValueVariant_defined( &restriction))
	{
		Reference<MetaDataRestrictionInterface> builder;
		builder.reset( storage->createMetaDataRestriction());
		if (!builder.get()) throw strus::runtime_error(_TXT("failed to create metadata restriction for %s"), ITERATOR_NAME);
		Deserializer::buildMetaDataRestriction( builder.get(), restriction, errorhnd);
		if (!m_postings.get())
		{
			m_postings.reset( storage->createBrowsePostingIterator( builder.get(), 1));
		}
		else
		{
			m_restriction.reset( builder->createInstance());
			if (!m_restriction.get()) throw strus::runtime_error(_TXT("failed to create metadata restriction for %s instance"), ITERATOR_NAME);
		}
	}
	std::vector<std::string> elemlist = Deserializer::getStringList( what);
	std::vector<std::string>::const_iterator ei = elemlist.begin(), ee = elemlist.end();
	for (; ei != ee; ++ei)
	{
		Index eh;
		if (utils::caseInsensitiveEquals( *ei, "position"))
		{
			m_items.push_back( ItemDef( *ei, ItemDef::Position));
		}
		else if (utils::caseInsensitiveEquals( *ei, "docno"))
		{
			m_items.push_back( ItemDef( *ei, ItemDef::Docno));
		}
		else if (0 <= (eh = metadatareader->elementHandle( ei->c_str())))
		{
			m_items.push_back( ItemDef( *ei, ItemDef::MetaData, eh));
		}
		else if (1 <= (eh = attributereader->elementHandle( ei->c_str())))
		{
			m_items.push_back( ItemDef( *ei, ItemDef::Attribute, eh));
		}
		else if (storage->termTypeNumber( *ei) != 0)
		{
			if (storage->isForwardIndexTerm( *ei))
			{
				Reference<ForwardIteratorInterface> fitr( storage->createForwardIterator( *ei));
				if (!fitr.get()) throw strus::runtime_error( _TXT("failed to create forward iterator for %s"), ITERATOR_NAME);
				m_forwarditer.push_back( fitr);
				m_items.push_back( ItemDef( *ei, ItemDef::ForwardIndex, m_forwarditer.size()-1));
			}
			else
			{
				Reference<DocumentTermIteratorInterface> titr( storage->createDocumentTermIterator( *ei));
				if (!titr.get()) throw strus::runtime_error( _TXT("failed to create search iterator for %s"), ITERATOR_NAME);
				m_searchiter.push_back( titr);
				m_items.push_back( ItemDef( *ei, ItemDef::SearchIndex, m_searchiter.size()-1));
			}
		}
		else
		{
			m_items.push_back( ItemDef( *ei, ItemDef::None));
		}
	}
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( _TXT("failed to instantiate %s: %s"), ITERATOR_NAME, errorhnd->fetchError());
	}
}

bool SelectIterator::buildRow( papuga_CallResult* result)
{
	AttributeReaderInterface* attributereader = 0;
	MetaDataReaderInterface* metadatareader = 0;

	bool ser = true;
	if (!papuga_set_CallResult_serialization( result)) throw std::bad_alloc();
	papuga_Serialization* serialization = result->value.value.serialization;
	if (m_items.empty())
	{
		ser &= papuga_Serialization_pushValue_int( serialization, m_docno);
	}
	else
	{
		ser &= papuga_Serialization_pushOpen( serialization);
		std::vector<ItemDef>::const_iterator ei = m_items.begin(), ee = m_items.end();
		for (; ei != ee; ++ei)
		{
			ser &= papuga_Serialization_pushName_string( serialization, ei->name().c_str(), ei->name().size());
			switch (ei->type())
			{
				case ItemDef::None:
					ser &= papuga_Serialization_pushValue_void( serialization);
					break;

				case ItemDef::MetaData:
					if (!metadatareader)
					{
						metadatareader = m_metadatareader_impl.getObject<MetaDataReaderInterface>();
						metadatareader->skipDoc( m_docno);
					}
					if (ei->handle() >= 0)
					{
						ser &= Serializer::serialize_nothrow( serialization, metadatareader->getValue( ei->handle()));
						attributereader = m_attributereader_impl.getObject<AttributeReaderInterface>();
					}
					else
					{
						ser &= papuga_Serialization_pushValue_void( serialization);
					}
					break;
				case ItemDef::Attribute:
					if (ei->handle() > 0)
					{
						if (!attributereader)
						{
							attributereader = m_attributereader_impl.getObject<AttributeReaderInterface>();
							attributereader->skipDoc( m_docno);
						}
						std::string attrvalstr = attributereader->getValue( ei->handle());
						const char* attrvalptr = papuga_Allocator_copy_string( &result->allocator, attrvalstr.c_str(), attrvalstr.size());
						if (!attrvalptr) throw std::bad_alloc();

						ser &= papuga_Serialization_pushValue_string( serialization, attrvalptr, attrvalstr.size());
					}
					else
					{
						ser &= papuga_Serialization_pushValue_void( serialization);
					}
					break;
				case ItemDef::ForwardIndex:
				{
					ForwardIteratorInterface* fitr = m_forwarditer[ ei->handle()].get();
					fitr->skipDoc( m_docno);
					Index pos = 0;
					std::string value;
					while (0!=(pos=fitr->skipPos( pos+1)))
					{
						if (!value.empty()) value.push_back('\t');
						value.append( fitr->fetch());
					}
					const char* valueptr = papuga_Allocator_copy_string( &result->allocator, value.c_str(), value.size());
					ser &= papuga_Serialization_pushValue_string( serialization, valueptr, value.size());
					break;
				}
				case ItemDef::SearchIndex:
				{
					DocumentTermIteratorInterface* titr = m_searchiter[ ei->handle()].get();
					if (titr->skipDoc( m_docno) == m_docno)
					{
						DocumentTermIteratorInterface::Term term;
						std::string value;
						while (titr->nextTerm( term))
						{
							if (!value.empty()) value.push_back('\t');
							value.append( titr->termValue( term.termno));
						}
						const char* valueptr = papuga_Allocator_copy_string( &result->allocator, value.c_str(), value.size());
						ser &= papuga_Serialization_pushValue_string( serialization, valueptr, value.size());
					}
					else
					{
						ser &= papuga_Serialization_pushValue_void( serialization);
					}
					break;
				}
				case ItemDef::Position:
					if (m_postings.get())
					{
						ser &= papuga_Serialization_pushOpen( serialization);
						for (Index pos = 0; 0!=(pos=m_postings->skipPos(pos)); ++pos)
						{
							ser &= papuga_Serialization_pushValue_int( serialization, pos);
						}
						ser &= papuga_Serialization_pushClose( serialization);
					}
					else
					{
						ser &= papuga_Serialization_pushValue_void( serialization);
					}
					break;
				case ItemDef::Docno:
					ser &= papuga_Serialization_pushValue_int( serialization, m_docno);
					break;
			}
		}
		ser &= papuga_Serialization_pushClose( serialization);
	}
	if (!ser)
	{
		papuga_CallResult_reportError( result, _TXT("memory allocation error in %s get next"), ITERATOR_NAME);
		return false;
	}
	return true;
}

bool SelectIterator::getNext( papuga_CallResult* result)
{
	try
	{
		if (m_postings.get())
		{
			if (!m_docno) return false;
			for (; 0!=(m_docno = m_postings->skipDoc( m_docno)); ++m_docno)
			{
				if (!m_restriction.get() || m_restriction->match( m_docno)) break;
			}
		}
		else
		{
			if (!m_docno || m_docno > m_maxdocno) return false;
		}
		bool rt = false;
		if (m_docno)
		{
			rt = buildRow( result);
			++m_docno;
		}
		ErrorBufferInterface* errorhnd = m_errorhnd_impl.getObject<ErrorBufferInterface>();
		if (errorhnd->hasError())
		{
			papuga_CallResult_reportError( result, _TXT("error in %s get next: %s"), ITERATOR_NAME, errorhnd->fetchError());
			rt = false;
		}
		return rt;
	}
	catch (const std::bad_alloc& err)
	{
		papuga_CallResult_reportError( result, _TXT("memory allocation error in %s get next"), ITERATOR_NAME);
		return false;
	}
	catch (const std::runtime_error& err)
	{
		papuga_CallResult_reportError( result, _TXT("error in %s get next: %s"), ITERATOR_NAME, err.what());
		return false;
	}
}

bool SelectIterator::GetNext( void* self, papuga_CallResult* result)
{
	return ((SelectIterator*)self)->getNext( result);
}

void SelectIterator::Deleter( void* obj)
{
	delete (SelectIterator*)obj;
}


