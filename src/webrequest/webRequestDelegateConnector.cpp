/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Implementation of the context of a delegated request (sub request to another server)
/// \file "webRequestDelegateConnector.cpp"
#include "webRequestDelegateConnector.hpp"
#include "strus/reference.hpp"
#include "strus/webRequestContent.hpp"
#include "strus/webRequestDelegateContextInterface.hpp"
#include "strus/errorCodes.hpp"
#include "private/internationalization.hpp"
#include "private/errorUtils.hpp"

using namespace strus;

WebRequestDelegateConnectionRef WebRequestDelegateConnector::getConnection( const std::string& address)
{
	strus::unique_lock lock( m_mutex);
	ConnectionMap::const_iterator ci = m_connectionMap.find( address);
	return ci == m_connectionMap.end() ? WebRequestDelegateConnectionRef() : ci->second;
}

void WebRequestDelegateConnector::setConnection( const std::string& address, const WebRequestDelegateConnectionRef& ref)
{
	strus::unique_lock lock( m_mutex);
	m_connectionMap[ address] = ref;
}

WebRequestDelegateConnectionRef WebRequestDelegateConnector::getOrCreateConnection( const std::string& address)
{
	WebRequestDelegateConnectionRef rt = getConnection( address);
	if (!rt.get())
	{
		rt.reset( new WebRequestDelegateConnection( address, m_eventloop));
		setConnection( address, rt);
	}
	return rt;
}

void WebRequestDelegateConnector::send( const std::string& address, const std::string& method, const std::string& content, WebRequestDelegateContextInterface* receiver)
{
	strus::shared_ptr<WebRequestDelegateContextInterface> receiverRef( receiver);
	WebRequestDelegateConnectionRef conn;
	try
	{
		conn = getOrCreateConnection( address);
		conn->push( method, content, receiverRef);
		WebRequestDelegateDataRef data( conn->fetch());
		if (data.get())
		{
			m_eventloop->pushJob( new WebRequestDelegateJob( conn, data, m_eventloop));
		}
	}
	catch (const std::runtime_error& err)
	{
		WebRequestAnswer answer( err.what(), 500, ErrorCodeDelegateRequestFailed);
		receiver->putAnswer( answer);
		m_eventloop->handleException( err.what());
		if (conn.get()) conn->dropAllPendingRequests(  err.what());
	}
	catch (const std::bad_alloc& )
	{
		WebRequestAnswer answer( _TXT("memory allocation error"), 500, ErrorCodeOutOfMem);
		receiver->putAnswer( answer);
		m_eventloop->handleException( _TXT("memory allocation error"));
		if (conn.get()) conn->dropAllPendingRequests( _TXT("memory allocation error"));
	}
	catch (...)
	{
		WebRequestAnswer answer( _TXT("unexpected exception"), 500, ErrorCodeDelegateRequestFailed);
		receiver->putAnswer( answer);
		m_eventloop->handleException( _TXT("unexpected exception"));
		if (conn.get()) conn->dropAllPendingRequests( _TXT("unexpected exception"));
	}
}


