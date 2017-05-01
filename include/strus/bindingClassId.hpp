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
	ClassNone=0x00,
	ClassContext=0x01,
	ClassDocumentAnalyzer=0x02,
	ClassQueryAnalyzer=0x03,
	ClassQueryEval=0x04,
	ClassQuery=0x05,
	ClassStorageClient=0x06,
	ClassStorageTransaction=0x07,
	ClassStatisticsProcessor=0x08,
	ClassStatisticsIterator=0x09,
	ClassDocumentBrowser=0x0a,
	ClassVectorStorageSearcher=0x0b,
	ClassVectorStorageClient=0x0c,
	ClassVectorStorageTransaction=0x0d
};

}//namespace
#endif

