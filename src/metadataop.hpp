/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef _STRUS_BINDING_METADATA_COMPAREOP_HPP_INCLUDED
#define _STRUS_BINDING_METADATA_COMPAREOP_HPP_INCLUDED
#include "strus/metaDataRestrictionInterface.hpp"

namespace strus {
namespace bindings {

MetaDataRestrictionInterface::CompareOperator getCompareOp( const char* compareOp);

}}
#endif
