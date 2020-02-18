/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDING_IMPL_STORAGE_VALUE_ITERATOR_HPP_INCLUDED
#define _STRUS_BINDING_IMPL_STORAGE_VALUE_ITERATOR_HPP_INCLUDED
#include "papuga/valueVariant.h"
#include "strus/storage/index.hpp"
#include "strus/reference.hpp"
#include "strus/valueIteratorInterface.hpp"
#include "impl/value/objectref.hpp"
#include <vector>
#include <string>

namespace strus {
namespace bindings {

class ValueIterator
{
public:
	enum {MaxNofElements=128};

	ValueIterator(
		const ObjectRef& trace_,
		const ObjectRef& objbuilder_,
		const ObjectRef& storage_,
		const ObjectRef& values_,
		const ObjectRef& errorhnd_,
		const std::string& key=std::string());
	virtual ~ValueIterator(){}

	bool getNext( papuga_CallResult* result);

	static bool GetNext( void* self, papuga_CallResult* result);
	static void Deleter( void* obj);

private:
	ObjectRef m_trace_impl;
	ObjectRef m_objbuilder_impl;
	ObjectRef m_storage_impl;
	ObjectRef m_values;
	ObjectRef m_errorhnd_impl;
	std::vector<std::string> m_block;
	std::vector<std::string>::const_iterator m_blockitr;
};

}}//namespace
#endif


