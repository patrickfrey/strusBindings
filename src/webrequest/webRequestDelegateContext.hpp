/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Implementation of the interface for executing XML/JSON requests on the strus bindings
/// \file "webRequestDelegateContext.hpp"
#ifndef _STRUS_WEB_REQUEST_DELEGATE_CONTEXT_IMPL_HPP_INCLUDED
#define _STRUS_WEB_REQUEST_DELEGATE_CONTEXT_IMPL_HPP_INCLUDED
#include "strus/webRequestDelegateContextInterface.hpp"
#include "strus/reference.hpp"
#include "papugaLuaRequestHandlerRef.hpp"

namespace strus {

class WebRequestDelegateContext
	:public PapugaLuaRequestHandlerRef
	,public WebRequestDelegateContextInterface
{
public:
	WebRequestDelegateContext(){}
	WebRequestDelegateContext( const PapugaLuaRequestHandlerRef& p, int idx_, const strus::Reference<int>& counter_) :PapugaLuaRequestHandlerRef(p),m_idx(idx_),m_counter(counter_){}
	WebRequestDelegateContext( const WebRequestDelegateContext& o) :PapugaLuaRequestHandlerRef(o),m_idx(o.m_idx),m_counter(o.m_counter){}
	virtual ~WebRequestDelegateContext(){}

	WebRequestDelegateContext& operator=( const WebRequestDelegateContext& o)
	{
		PapugaLuaRequestHandlerRef::operator=(o); m_idx=o.m_idx; return *this;
	}

	void reset( papuga_LuaRequestHandler* ptr_, int idx_)
	{
		PapugaLuaRequestHandlerRef::reset( ptr_);
		m_idx = idx_;
	}

	void putAnswer( const WebRequestAnswer& status)
	{
		*m_counter -= 1;
		if (status.ok())
		{
			papuga_LuaRequestHandler_init_result( PapugaLuaRequestHandlerRef::get(), m_idx, status.content().str(), status.content().len());
		}
		else
		{
			papuga_LuaRequestHandler_init_error( PapugaLuaRequestHandlerRef::get(), m_idx, errorCodeToPapugaError( (strus::ErrorCode)status.appErrorCode()), status.errorStr());
		}
	}

private:
	int m_idx;
	strus::Reference<int> m_counter;
};

}//namespace
#endif


