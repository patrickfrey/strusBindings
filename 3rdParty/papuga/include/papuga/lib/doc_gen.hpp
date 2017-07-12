/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
/// \brief Library interface for libpapuga_doc for generating documentation out of a doxygen like syntax with help of some templates
/// \file papuga/lib/doc_gen.hpp
#ifndef _PAPUGA_DOC_GEN_LIB_HPP_INCLUDED
#define _PAPUGA_DOC_GEN_LIB_HPP_INCLUDED
#include <string>
#include <map>
#include <iostream>

namespace papuga {

/// \brief Generate a documentation out of a doxygen like syntax with help of some templates
/// \param[out] out where to print the generated item
/// \param[err] err stream to report errors and warnings
/// \param[in] templatesrc source of the templates
/// \param[in] docsrc source to map
/// \param[in] varmap variables initialized by the caller
bool generateDoc(
	std::ostream& out,
	std::ostream& err,
	const std::string& templatesrc,
	const std::string& docsrc,
	const std::map<std::string,std::string>& varmap);

}//namespace
#endif
