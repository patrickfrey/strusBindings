/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _PAPUGA_ITERATOR_H_INCLUDED
#define _PAPUGA_ITERATOR_H_INCLUDED
/// \brief Representation of an iterator in the host environment of the papuga language bindings
/// \file iterator.h
#include "papuga/typedefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Constructor of an iterator reference
/// \param[out] self pointer to structure initialized by constructor
/// \param[in] object_ pointer to iterator context object
/// \param[in] destroy_ destructor of the iterated object in case of ownership
/// \param[in] getNext_ method of the iterated object to fetch the next element
#define papuga_init_Iterator( self, object_, destroy_, getNext_)	{(self)->data=object_; (self)->destroy=destroy_; (self)->getNext=getNext_;}

/// \brief Release of ownership of an iterated object reference
/// \param[in,out] self pointer to structure
#define papuga_release_Iterator( self)					{(self)->destroy=0;}

/// \brief Destructor of an iterated object reference
/// \param[in,out] self pointer to structure
#define papuga_destroy_Iterator( self)					{if ((self)->destroy) {(self)->destroy( (self)->data);(self)->destroy=0;}}

#ifdef __cplusplus
}
#endif
#endif

