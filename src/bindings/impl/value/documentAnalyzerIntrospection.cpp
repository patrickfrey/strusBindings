/*
 * Copyright (c) 2018 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Interface for introspection of a document analyzer
#include "documentAnalyzerIntrospection.hpp"
#include "introspectionTemplates.hpp"
#include "serializer.hpp"
#include "private/internationalization.hpp"
#include "strus/constants.hpp"
#include "strus/base/stdint.h"
#include "strus/base/numstring.hpp"
#include "strus/base/local_ptr.hpp"
#include "strus/base/configParser.hpp"
#include "strus/valueIteratorInterface.hpp"
#include "papuga/allocator.h"
#include "papuga/constants.h"
#include <cstring>
#include <vector>
#include <set>
#include <utility>
#include <cstdint>
#include <limits>

using namespace strus;
using namespace strus::bindings;

void DocumentAnalyzerIntrospection::serialize( papuga_Serialization& serialization, const std::string& path)
{
	serializeMembers( serialization, path);
}

IntrospectionBase* DocumentAnalyzerIntrospection::open( const std::string& name)
{
	if (name == "feature") return NULL;
	else if (name == "subdoc") return NULL;
	else if (name == "subcontent") return NULL;
	else if (name == "patternmatcher") return NULL;
	else if (name == "patternlexer") return NULL;
	else if (name == "segmenter") return NULL;
	return NULL;
}

std::vector<IntrospectionLink> DocumentAnalyzerIntrospection::list()
{
	static const char* ar[] = {".feature",".subdoc",".subcontent",".patternmatcher",".patternlexer",".segmenter",NULL};
	return getList( ar);
}


