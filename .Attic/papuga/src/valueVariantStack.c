/*
 * Copyright (c) 2017 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Stack of variant values
/// \file valueVariantStack.c
#include "papuga/valueVariant.h"
#include <stdlib>

void papuga_destroy_ValueVariantStack( papuga_ValueVariantStack* stk)
{
	if (stk->cur != &stk->main)
	{
		if (stk->prev) papuga_destroy_ValueVariantStack( stk->prev);
		free( stk->cur);
	}
}

bool papuga_ValueVariantStack_push( papuga_ValueVariantStack* stk, const papuga_ValueVariant* elem)
{
	if (stk->cur->size == (sizeof(stk->cur->ar) / sizeof(stk->cur->ar[0])))
	{
		papuga_ValueVariantStackChunk* chunk = (papuga_ValueVariantStackChunk*)malloc( sizeof( papuga_ValueVariantStackChunk));
		if (!chunk) return false;
		chunk->prev = stk->cur;
		chunk->arsize = 0;
		stk->cur = chunk;
	}
	papuga_init_ValueVariant_copy( &stk->cur->ar[ stk->cur->size++], elem);
	return true;
}

const papuga_ValueVariant* papuga_ValueVariantStack_pop( papuga_ValueVariantStack* stk)
{
	papuga_ValueVariant* rt = 0;
	if (stk->cur->size == 0)
	{
		if (!stk->cur->prev) return 0;
		papuga_ValueVariantStackChunk* prev = stk->cur->prev;
		free( stk->cur);
		stk->cur = stk->cur->prev;
	}
	rt = stk->cur->ar[ stk->cur->size--];
	return rt;
}

