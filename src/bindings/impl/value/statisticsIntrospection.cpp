/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Interface for introspection of a global term statistics map
#include "introspectionBase.hpp"
#include "statisticsIntrospection.hpp"
#include "introspectionTemplates.hpp"
#include "strus/statisticsStorageClientInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/base/stdint.h"

using namespace strus;
using namespace strus::bindings;

void StatisticsStorageIntrospection::serialize( papuga_Serialization& serialization, bool substructure)
{
	serializeMembers( serialization, substructure);
}

IntrospectionBase* StatisticsStorageIntrospection::open( const std::string& name)
{
	return nullptr;
}

std::vector<IntrospectionLink> StatisticsStorageIntrospection::list()
{
	std::vector<IntrospectionLink> rt;
	return rt;
}

