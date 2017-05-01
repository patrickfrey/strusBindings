/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _PAPUGA_HOST_OBJECT_REFERENCE_H_INCLUDED
#define _PAPUGA_HOST_OBJECT_REFERENCE_H_INCLUDED
/// \brief Representation of an object in the host environment of the papuga language bindings
/// \file hostObjectReference.h
#include "papuga/typedefs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define papuga_init_HostObjectReference( self, object_, destroy_)	{(self)->data=object_; (self)->destroy=destroy_;}
#define papuga_destroy_HostObjectReference( self)			{if ((self)->destroy) {(self)->destroy( (self)->data);(self)->destroy=0;}}

#ifdef __cplusplus
}
#endif
#endif

