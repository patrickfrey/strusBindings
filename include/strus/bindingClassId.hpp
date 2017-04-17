/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_BINDINGS_CLASS_IDENTIFIER_UTILS_HPP_INCLUDED
#define _STRUS_BINDINGS_CLASS_IDENTIFIER_UTILS_HPP_INCLUDED
/// \brief Identifiers for classes asseociated with exported method tables for calling strus
/// \file bindingClassId.hpp

namespace strus {

enum BindingsClassId
{
	ClassContext=0x0,
	ClassDocumentAnalyzer=0x1,
	ClassQueryAnalyzer=0x2
};

}//namespace
#endif

