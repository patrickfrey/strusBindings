/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _PAPUGA_VALUE_VARIANT_STACK_H_INCLUDED
#define _PAPUGA_VALUE_VARIANT_STACK_H_INCLUDED
/// \brief Stack of variant values
/// \file valueVariantStack.h
#include "papuga/valueVariant.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct papuga_ValueVariantStackChunk
{
	papuga_ValueVariantStack* prev;
	papuga_ValueVariant ar[ 1024];
	size_t arsize;
} papuga_ValueVariantStackChunk;

typedef struct papuga_ValueVariantStack
{
	papuga_ValueVariantStackChunk main;
	struct papuga_ValueVariantStack* cur;
} papuga_ValueVariantStack;

#define papuga_init_ValueVariantStack(stk)				{(stk)->main.prev=0;(stk)->main.arsize=0;(stk)->cur=&(stk)->main;}
void papuga_destroy_ValueVariantStack( papuga_ValueVariantStack* stk);

bool papuga_ValueVariantStack_push( papuga_ValueVariantStack* stk, const papuga_ValueVariant* elem);
const papuga_ValueVariant* papuga_ValueVariantStack_pop( papuga_ValueVariantStack* stk);

#ifdef __cplusplus
}
#endif
#endif


