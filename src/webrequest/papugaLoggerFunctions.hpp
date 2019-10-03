/*
 * Copyright (c) 2019 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/* @brief Logging functions for papuga wrapping to web request logging calls
 * @file papugaLoggerFunctions.hpp
 */
#ifndef _STRUS_WEBREQUEST_PAPUGA_LOGGER_FUNCTIONS_HPP_INCLUDED
#define _STRUS_WEBREQUEST_PAPUGA_LOGGER_FUNCTIONS_HPP_INCLUDED
#include "papuga/typedefs.h"

namespace strus {

void papugaLogMethodCall( void* self_/*WebRequestLoggerInterface*/, int nofItems, ...);
void papugaLogContentEvent( void* self_/*WebRequestLoggerInterface*/, const char* title, int itemid, const papuga_ValueVariant* value);

} //namespace
#endif
