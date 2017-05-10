/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _PAPUGA_ERRORS_H_INCLUDED
#define _PAPUGA_ERRORS_H_INCLUDED
/// \brief Mapping of error codes to strings
/// \file errors.h

#ifdef __cplusplus
extern "C" {
#endif

const char* papuga_ErrorCode_tostring( int errorcode);

#ifdef __cplusplus
}
#endif
#endif

