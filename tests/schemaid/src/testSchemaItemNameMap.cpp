/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "schemas_base.hpp"
#include <iostream>

typedef strus::webrequest::AutomatonNameSpace NS;
typedef NS::Item Item;

int main( int argc, const char* argv[])
{
	try
	{
		Item ii = NS::NullValue;
		Item ie = NS::NofItemDefinitions;
		for (; ii != ie; ii = (Item)((int)ii+1))
		{
			std::cerr << NS::itemName( ii) << std::endl;
		}
		std::cerr << std::endl << "done" << std::endl;
	}
	catch (...)
	{
		return -1;
	}
	return 0;
}

