/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_IMPL_STORAGE_SELECT_ITERATOR_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_STORAGE_SELECT_ITERATOR_HPP_INCLUDED
#include "papuga/valueVariant.h"
#include "strus/index.hpp"
#include "strus/reference.hpp"
#include "strus/storageClientInterface.hpp"
#include "strus/postingIteratorInterface.hpp"
#include "strus/metaDataRestrictionInstanceInterface.hpp"
#include "strus/forwardIteratorInterface.hpp"
#include "strus/documentTermIteratorInterface.hpp"
#include "strus/invAclIteratorInterface.hpp"
#include "impl/value/objectref.hpp"
#include "internationalization.hpp"
#include <vector>
#include <string>

namespace strus {
namespace bindings {

class SelectIterator
{
public:
	SelectIterator(
		const ObjectRef& trace_,
		const ObjectRef& objbuilder_,
		const ObjectRef& storage_,
		const ObjectRef& errorhnd_,
		const papuga_ValueVariant& what,
		const papuga_ValueVariant& expression,
		const papuga_ValueVariant& restriction,
		const Index& start_docno_,
		const papuga_ValueVariant& accesslist);
	virtual ~SelectIterator(){}

	bool getNext( papuga_CallResult* result);

	static bool GetNext( void* self, papuga_CallResult* result);
	static void Deleter( void* obj);

private:
	bool buildRow( papuga_CallResult* result);

private:
	class ItemDef
	{
	public:
		enum Type {
			None,
			MetaData,
			Attribute,
			ForwardIndex,
			SearchIndex,
			Position,
			Docno
		};

		ItemDef( const std::string& name_, const Type& type_, const Index& handle_=-1)
			:m_name(name_),m_type(type_),m_handle(handle_){}
		ItemDef( const ItemDef& o)
			:m_name(o.m_name),m_type(o.m_type),m_handle(o.m_handle){}

		const std::string& name() const		{return m_name;}
		const Type& type() const		{return m_type;}
		const Index& handle() const		{return m_handle;}

	private:
		std::string m_name;
		Type m_type;
		Index m_handle;
	};

	struct AccessRestriction
	{
		Index docno;
		Reference<InvAclIteratorInterface> acciter;

		AccessRestriction( const StorageClientInterface* storage, const std::string& ac)
			:docno(0),acciter( storage->createInvAclIterator( ac))
		{
			if (!acciter.get()) throw strus::runtime_error(_TXT("failed to create access restriction for '%s'"), ac.c_str());
			docno = acciter->skipDoc( 0);
		}
		AccessRestriction( const AccessRestriction& o)
			:docno(o.docno),acciter(o.acciter){}
	};
private:
	bool checkAccess( const Index& docno);

private:
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_storage_impl;
	ObjectRef m_attributereader_impl;
	ObjectRef m_metadatareader_impl;
	ObjectRef m_errorhnd_impl;
	Reference<PostingIteratorInterface> m_postings;
	Reference<MetaDataRestrictionInstanceInterface> m_restriction;
	std::vector<Reference<ForwardIteratorInterface> > m_forwarditer;
	std::vector<Reference<DocumentTermIteratorInterface> > m_searchiter;
	std::vector<AccessRestriction> m_accessiter;
	Index m_docno;
	Index m_maxdocno;
	std::vector<ItemDef> m_items;
};

}}//namespace
#endif
