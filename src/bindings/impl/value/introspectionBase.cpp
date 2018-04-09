/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Abstract interface for introspection
#include "introspectionBase.hpp"
#include "private/internationalization.hpp"
#include "papuga/allocator.h"
#include "papuga/constants.h"
#include "papuga/serialization.h"
#include "serializer.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/errorCodes.hpp"
#include "strus/valueIteratorInterface.hpp"
#include "strus/base/local_ptr.hpp"
#include <stdexcept>

using namespace strus;
using namespace strus::bindings;

std::vector<IntrospectionLink> IntrospectionLink::getList( bool autoexpand_, const std::vector<std::string>& values)
{
	std::vector<IntrospectionLink> rt;
	std::vector<std::string>::const_iterator vi = values.begin(), ve = values.end();
	for (; vi != ve; ++vi)
	{
		rt.push_back( IntrospectionLink( autoexpand_, *vi));
	}
	return rt;
}

std::runtime_error IntrospectionBase::unresolvable_exception()
{
	return strus::runtime_error( ErrorCodeRequestResolveError, _TXT("not found"));
}

std::vector<IntrospectionLink> IntrospectionBase::getList( const char** ar)
{
	std::vector<IntrospectionLink> rt;
	char const** ai = ar;
	for (; *ai; ++ai)
	{
		if (**ai == '.')
		{
			rt.push_back( IntrospectionLink( false, *ai+1));
		}
		else
		{
			rt.push_back( IntrospectionLink( true, *ai));
		}
	}
	return rt;
}

std::vector<std::string> IntrospectionBase::getKeyList( const std::vector<std::pair<std::string,std::string> >& ar)
{
	std::vector<std::string> rt;
	typename std::vector<std::pair<std::string,std::string> >::const_iterator ci = ar.begin(), ce = ar.end();
	for (; ci != ce; ++ci)
	{
		rt.push_back( ci->first);
	}
	return rt;
}

void IntrospectionBase::serializeMembers( papuga_Serialization& serialization, const std::string& path)
{
	std::vector<IntrospectionLink> elems = this->list();
	std::vector<IntrospectionLink>::const_iterator li = elems.begin(), le = elems.end();
	for (; li != le; ++li)
	{
		if (li->autoexpand())
		{
			strus::local_ptr<IntrospectionBase> introspection( open( li->value()));
			if (introspection.get())
			{
				introspection->serializeStructureAs( serialization, li->value().c_str(), path.empty() ? li->value() : (path + "/" + li->value()));
			}
		}
		else
		{
			Serializer::serializeWithName( &serialization, PAPUGA_HTML_LINK_ELEMENT, path.empty() ? li->value() : (path + "/" + li->value()), true);
		}
	}
}

void IntrospectionBase::serializeStructureAs( papuga_Serialization& serialization, const char* name, const std::string& path)
{
	bool sc = true;
	const char* namecopy = papuga_Allocator_copy_charp( serialization.allocator, name);
	if (!namecopy) throw std::bad_alloc();
	sc &= papuga_Serialization_pushName_charp( &serialization, namecopy);
	sc &= papuga_Serialization_pushOpen( &serialization);
	this->serialize( serialization, path);
	sc &= papuga_Serialization_pushClose( &serialization);
	if (!sc) throw std::bad_alloc();
}

void IntrospectionBase::getPathContent( papuga_Serialization& serialization, const std::vector<std::string>& path)
{
	strus::local_ptr<IntrospectionBase> ictx;
	IntrospectionBase* cur = this;
	std::vector<std::string>::const_iterator pi = path.begin(), pe = path.end();
	std::string openedpath;
	for (; pi != pe; ++pi)
	{
		ictx.reset( cur->open( *pi));
		if (!ictx.get())
		{
			throw strus::runtime_error( ErrorCodeRequestResolveError, _TXT("/%s not found in %s%s"), pi->c_str(), "/context", openedpath.c_str());
		}
		cur = ictx.get();
		openedpath.push_back('/');
		openedpath.append( *pi);
	}
	cur->serialize( serialization, std::string());
}

IntrospectionValueIterator::IntrospectionValueIterator( ErrorBufferInterface* errorhnd_, const strus::Reference<ValueIteratorInterface>& impl_, bool prefixBound_, const std::string& name_)
	:m_errorhnd(errorhnd_),m_impl(impl_),m_name(name_),m_prefixBound(prefixBound_)
{
	if (m_prefixBound)
	{
		m_impl->skipPrefix( m_name.c_str(), m_name.size());
	}
	else
	{
		m_impl->skip( m_name.c_str(), m_name.size());
	}
}

void IntrospectionValueIterator::serialize( papuga_Serialization& serialization, const std::string& path)
{
	std::vector<std::string> valuelist = m_impl->fetchValues( MaxListSizeDeepExpansion);
	Serializer::serialize( &serialization, valuelist, true/*deep*/);
}
IntrospectionBase* IntrospectionValueIterator::open( const std::string& name_)
{
	if (m_name.empty())
	{
		return new IntrospectionValueIterator( m_errorhnd, m_impl, m_prefixBound, name_);
	}
	else
	{
		return NULL;
	}
}
std::vector<IntrospectionLink> IntrospectionValueIterator::list()
{
	return IntrospectionLink::getList( false, m_impl->fetchValues( MaxListSizeDeepExpansion));
}


