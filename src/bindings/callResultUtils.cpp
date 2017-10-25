/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Helper functions to build the CallResult structure, the uniform return value of binding methods
/// \file callResultUtils.cpp
#include "callResultUtils.hpp"

void strus::initCallResultStruct( papuga_CallResult* retval, const strus::bindings::Struct& st)
{
	papuga_Allocator* allocator = papuga_Allocator_alloc_Allocator( &retval->allocator);
	if (!allocator) std::bad_alloc();
	std::memcpy( allocator, &st.allocator, sizeof(st.allocator));
	if (!papuga_add_CallResult_serialization( retval)) throw std::bad_alloc();
	std::memcpy( retval->valuear[0].value.serialization, &st.serialization, sizeof(papuga_Serialization));
}

void strus::initCallResultIterator( papuga_CallResult* retval, const strus::bindings::Iterator& st)
{
	if (!papuga_add_CallResult_iterator( retval, st.iterator.data, st.iterator.destroy, st.iterator.getNext)) throw std::bad_alloc();
}


