/*
* Copyright (c) 2017 Patrick P. Frey
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
/// \brief Some utility functions for generating language binding sources
/// \file private/gen_utils.hpp
#ifndef _PAPUGA_UTILS_SOURCE_DOC_HPP_INCLUDED
#define _PAPUGA_UTILS_SOURCE_DOC_HPP_INCLUDED
#include "papuga/interfaceDescription.h"
#include <string>
#include <map>
#include <stdexcept>

namespace papuga {

class SourceDocLanguageDescription
{
public:
	virtual const char* eolncomment() const=0;
	virtual std::string mapCodeExample( const std::string& example) const=0;
	virtual std::string constructorDeclaration( const std::string& classname, const papuga_ConstructorDescription* cdef) const=0;
	virtual std::string methodDeclaration( const std::string& classname, const papuga_MethodDescription* mdef) const=0;
private:
};

void printSourceDoc( std::ostream& out, const SourceDocLanguageDescription* lang, const papuga_InterfaceDescription& descr);

}//namespace
#endif

