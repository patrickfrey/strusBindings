/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* @brief RAII Container for papuga request context
 * @file papugaContextRef.hpp
 */
#ifndef _STRUS_WEBREQUEST_PAPUGA_CONTEXTREF_HPP_INCLUDED
#define _STRUS_WEBREQUEST_PAPUGA_CONTEXTREF_HPP_INCLUDED
#include "papuga/requestHandler.h"
#include "strus/reference.hpp"
#include <iostream>

namespace strus {

class PapugaRequestContextDeleter
{
public:
	void operator()( papuga_RequestContext* p)
	{
		papuga_destroy_RequestContext(p);
	}
};

class PapugaContextRef
	:public strus::Reference<papuga_RequestContext,PapugaRequestContextDeleter>
{
public:
	typedef strus::Reference<papuga_RequestContext,PapugaRequestContextDeleter> Parent;
	PapugaContextRef(){}
	explicit PapugaContextRef( papuga_RequestContext* p) :Parent(p){}
	PapugaContextRef( const PapugaContextRef& o) :Parent(o){}
	PapugaContextRef& operator=( const PapugaContextRef& o) {Parent::operator=(o); return *this;}

	void create() {Parent::operator=( Parent( papuga_create_RequestContext()));}
};

}//namespace
#endif

