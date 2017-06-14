/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _PAPUGA_HOST_OBJECT_H_INCLUDED
#define _PAPUGA_HOST_OBJECT_H_INCLUDED
/// \brief Representation of an object in the host environment of the papuga language bindings
/// \file hostObject.h
#include "papuga/typedefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Constructor of a host object reference
/// \param[out] self pointer to structure initialized by constructor
/// \param[in] object_ pointer to host object
/// \param[in] destroy_ destructor of the host object in case of ownership
#define papuga_init_HostObject( self, classid_, object_, destroy_)	{(self)->classid=classid_; (self)->data=object_; (self)->destroy=destroy_;}

/// \brief Release of ownership of a host object reference
/// \param[in,out] self pointer to structure
#define papuga_release_HostObject( self)				{(self)->destroy=0;}

/// \brief Destructor of a host object reference
/// \param[in,out] self pointer to structure
#define papuga_destroy_HostObject( self)				{if ((self)->destroy) {(self)->destroy( (self)->data);(self)->destroy=0;}}

#ifdef __cplusplus
}
#endif
#endif

