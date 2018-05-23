/*
 * Copyright (c) 2018 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "inserter.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/documentAnalyzerInterface.hpp"
#include "strus/documentAnalyzerContextInterface.hpp"
#include "strus/analyzer/document.hpp"
#include "strus/analyzer/documentAttribute.hpp"
#include "strus/lib/bindings_description.hpp"
#include "strus/base/local_ptr.hpp"
#include "serializer.hpp"
#include "deserializer.hpp"
#include "papuga/allocator.h"
#include "papuga/serialization.h"

using namespace strus;
using namespace strus::bindings;

Struct InserterImpl::introspection( const ValueVariant& arg) const
{
	Struct rt;
	std::vector<std::string> path;
	if (papuga_ValueVariant_defined( &arg))
	{
		path = Deserializer::getStringList( arg);
	}
	strus::local_ptr<IntrospectionBase> ictx;
	if (path.empty())
	{
		static const char* ar[] = {"analyzer","storage", 0};
		ErrorBufferInterface* errorhnd = m_analyzer.m_errorhnd_impl.getObject<ErrorBufferInterface>();
		ictx.reset( new LinkIntrospection( errorhnd, ar));
		ictx->getPathContent( rt.serialization, path);
		if (errorhnd->hasError())
		{
			throw strus::runtime_error(_TXT( "failed to serialize introspection: %s"), errorhnd->fetchError());
		}
	}
	else if (path[0] == "analyzer" || path[0] == "storage")
	{
		papuga_Serialization ser;
		papuga_Allocator allocator;
		int allocator_mem[ 1024];
		papuga_init_Allocator( &allocator, &allocator_mem, sizeof(allocator_mem));
		papuga_init_Serialization( &ser, &allocator);
		path.erase( path.begin());
		Serializer::serialize( &ser, path, false/*deep*/);
		papuga_ValueVariant subarg;
		papuga_init_ValueVariant_serialization( &subarg, &ser);
		try
		{
			if (path[0] == "analyzer")
			{
				rt = m_analyzer.introspection( subarg);
			}
			else//if (path[0] == "storage")
			{
				rt = m_storage.introspection( subarg);
			}
		}
		catch (const std::bad_alloc&)
		{
			papuga_destroy_Allocator( &allocator);
			throw std::bad_alloc();
		}
		catch (const std::runtime_error& err)
		{
			papuga_destroy_Allocator( &allocator);
			throw std::runtime_error( err.what());
		}
		papuga_destroy_Allocator( &allocator);
	}
	else
	{
		throw strus::runtime_error_ec( ErrorCodeRequestResolveError, "unknown inserter item '%s'", path[0].c_str());
	}
	rt.release();
	return rt;
}

InserterTransactionImpl* InserterImpl::createTransaction() const
{
	strus::local_ptr<StorageTransactionImpl> transaction( m_storage.createTransaction());
	return new InserterTransactionImpl( transaction.get(), &m_analyzer);
}

void InserterTransactionImpl::insertDocument( const std::string& docid, const std::string& content, const ValueVariant& documentClass)
{
	const DocumentAnalyzerInterface* analyzer = m_analyzer.m_analyzer_impl.getObject<const DocumentAnalyzerInterface>();
	ErrorBufferInterface* errorhnd = m_analyzer.m_errorhnd_impl.getObject<ErrorBufferInterface>();
	analyzer::DocumentClass dclass = m_analyzer.getDocumentClass( content, documentClass);
	strus::local_ptr<DocumentAnalyzerContextInterface> analyzerContext( analyzer->createContext( dclass));
	if (!analyzerContext.get()) throw std::runtime_error( errorhnd->fetchError());
	analyzerContext->putInput( content.c_str(), content.size(), true/*eof*/);
	strus::local_ptr<analyzer::Document> doc( new analyzer::Document());
	int documentCount = 0;
	while (analyzerContext->analyzeNext( *doc))
	{
		papuga_Serialization docser;
		papuga_Allocator allocator;
		int allocator_mem[ 1024];
		papuga_init_Allocator( &allocator, &allocator_mem, sizeof(allocator_mem));
		papuga_init_Serialization( &docser, &allocator);
		
		try
		{
			Serializer::serialize( &docser, *doc, false/*deep*/);
			papuga_ValueVariant docval;
			papuga_init_ValueVariant_serialization( &docval, &docser);

			std::string id;
			if (docid.empty())
			{
				std::vector<analyzer::DocumentAttribute>::const_iterator ai = doc->attributes().begin(), ae = doc->attributes().end();
				for (; ai != ae; ++ai)
				{
					if (ai->name() == strus::Constants::attribute_docid())
					{
						id = ai->value();
						break;
					}
				}
				if (ai == ae)
				{
					throw strus::runtime_error(_TXT("insert document without docid or empty docid defined"));
				}
				m_transaction.insertDocument( id, docval);
				documentCount++;
			}
			else
			{
				m_transaction.insertDocument( docid, docval);
				documentCount++;
			}
		}
		catch (const std::bad_alloc&)
		{
			papuga_destroy_Allocator( &allocator);
			throw std::bad_alloc();
		}
		catch (const std::runtime_error& err)
		{
			papuga_destroy_Allocator( &allocator);
			throw err;
		}
		papuga_destroy_Allocator( &allocator);
		if (documentCount > 1 && !docid.empty())
		{
			throw strus::runtime_error(_TXT("specified docid for inserter to insert of a multipart document"));
		}
	}
	if (errorhnd->hasError())
	{
		throw strus::runtime_error( "%s", errorhnd->fetchError());
	}
}

