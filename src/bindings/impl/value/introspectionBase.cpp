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
#include "strus/base/fileio.hpp"
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
	return strus::runtime_error_ec( ErrorCodeRequestResolveError, _TXT("not found"));
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

void IntrospectionBase::serializeMembers( papuga_Serialization& serialization, bool substructure)
{
	int nofLinks = 0;
	std::vector<IntrospectionLink> elems = this->list();
	std::vector<IntrospectionLink>::const_iterator li = elems.begin(), le = elems.end();
	if (substructure && !papuga_Serialization_pushOpen( &serialization)) throw std::bad_alloc();

	for (; li != le; ++li)
	{
		if (li->autoexpand())
		{
			strus::local_ptr<IntrospectionBase> introspection( open( li->value()));
			if (introspection.get())
			{
				introspection->serializeStructureAs( serialization, li->value().c_str());
			}
		}
		else
		{
			++nofLinks;
		}
	}
	if (nofLinks)
	{
		if (!papuga_Serialization_pushName_charp( &serialization, PAPUGA_HTML_LINK_ELEMENT)
		||  !papuga_Serialization_pushOpen( &serialization)) throw std::bad_alloc();
		
		for (li = elems.begin(); li != le; ++li)
		{
			if (!li->autoexpand())
			{
				Serializer::serialize( &serialization, li->value(), true);
			}
		}
		if (!papuga_Serialization_pushClose( &serialization)) throw std::bad_alloc();
	}
	if (substructure && !papuga_Serialization_pushClose( &serialization)) throw std::bad_alloc();
}

void IntrospectionBase::serializeStructureAs( papuga_Serialization& serialization, const char* name)
{
	bool sc = true;
	const char* namecopy = papuga_Allocator_copy_charp( serialization.allocator, name);
	if (!namecopy) throw std::bad_alloc();
	sc &= papuga_Serialization_pushName_charp( &serialization, namecopy);
	this->serialize( serialization, true/*substructure*/);
	if (!sc) throw std::bad_alloc();
}

void IntrospectionBase::getPathContent( papuga_Serialization& serialization, const std::vector<std::string>& path, bool substructure)
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
			throw strus::runtime_error_ec( ErrorCodeRequestResolveError, _TXT("'%s' not found in /%s"), pi->c_str(), openedpath.c_str());
		}
		cur = ictx.get();
		openedpath.push_back('/');
		openedpath.append( *pi);
	}
	cur->serialize( serialization, substructure);
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

void IntrospectionValueIterator::serialize( papuga_Serialization& serialization, bool substructure)
{
	std::vector<std::string> valuelist = m_impl->fetchValues( MaxListSizeDeepExpansion);
	if (substructure)
	{
		bool sc = true;
		sc &= papuga_Serialization_pushOpen( &serialization);
		Serializer::serialize( &serialization, valuelist, true/*deep*/);
		sc &= papuga_Serialization_pushClose( &serialization);
		if (!sc) throw std::bad_alloc();
	}
	else
	{
		Serializer::serialize( &serialization, valuelist, true/*deep*/);
	}
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


IntrospectionDirectoryIterator::IntrospectionDirectoryIterator( ErrorBufferInterface* errorhnd_, const strus::Reference<ValueIteratorInterface>& impl_, const std::string& name_, bool complete_)
	:m_errorhnd(errorhnd_),m_impl(impl_),m_name(name_),m_complete(complete_)
{}

std::vector<std::string> IntrospectionDirectoryIterator::fetchValues()
{
	std::vector<std::string> rt;
	std::string pathprefix = m_name;
	if (m_complete && !m_name.empty()) pathprefix.push_back('/');
	std::string pathsearch = pathprefix;
	std::string pathcontext;
	if (!m_complete)
	{
		if (0!=std::strchr( m_name.c_str(), '/'))
		{
			int ec = strus::getParentPath( m_name, pathcontext);
			if (ec)
			{
				throw strus::runtime_error_ec( ErrorCodeRequestResolveError, _TXT("introspection failed: %s"), m_errorhnd->fetchError());
			}
			pathcontext.push_back( '/');
		}
	}
	else
	{
		pathcontext = pathprefix;
	}
	while (rt.size() < MaxListSizeDeepExpansion)
	{
		m_impl->skip( pathsearch.c_str(), pathsearch.size());
		std::vector<std::string> valuelist = m_impl->fetchValues( MaxListSizeDeepExpansion);
		if (valuelist.empty())
		{
			return rt;
		}
		std::vector<std::string>::const_iterator vi = valuelist.begin(), ve = valuelist.end();
		for (; vi != ve && rt.size() < MaxListSizeDeepExpansion; ++vi)
		{
			if (!strus::stringStartsWith( *vi, pathprefix))
			{
				return rt;
			}
			char const* elem = vi->c_str() + pathcontext.size();
			char const* delim = std::strchr( elem, '/');
			if (!delim)
			{
				rt.push_back( elem);
			}
			else
			{
				// Take only directory as element and set the key to skip all elements of this directory
				std::string elemstr( elem, delim-elem);
				rt.push_back( elemstr);
				pathsearch = pathcontext + elemstr + "\1";
				break;
			}
		}
		if (vi == ve)
		{
			// ... we are done as we did not reinitialize 'pathsearch'
			break;
		}
	}
	return rt;
}

void IntrospectionDirectoryIterator::serialize( papuga_Serialization& serialization, bool substructure)
{
	std::vector<std::string> valuelist = fetchValues();
	if (substructure)
	{
		bool sc = true;
		sc &= papuga_Serialization_pushOpen( &serialization);
		Serializer::serialize( &serialization, valuelist, true/*deep*/);
		sc &= papuga_Serialization_pushClose( &serialization);
		if (!sc) throw std::bad_alloc();
	}
	else
	{
		Serializer::serialize( &serialization, valuelist, true/*deep*/);
	}
}

IntrospectionBase* IntrospectionDirectoryIterator::open( const std::string& name_)
{
	if (m_name.empty())
	{
		return new IntrospectionDirectoryIterator( m_errorhnd, m_impl, name_, true);
	}
	else
	{
		return NULL;
	}
}
std::vector<IntrospectionLink> IntrospectionDirectoryIterator::list()
{
	return IntrospectionLink::getList( false, fetchValues());
}

void LinkIntrospection::serialize( papuga_Serialization& serialization, bool substructure)
{
	serializeMembers( serialization, substructure);
}



