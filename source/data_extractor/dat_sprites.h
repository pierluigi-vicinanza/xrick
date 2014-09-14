/*
 * xrick/data/dat_sprites.h
 *
 * Copyright (C) 2008-2014 Pierluigi Vicinanza. All rights reserved.
 *
 * The use and distribution terms for this software are contained in the file
 * named README, which can be found in the root of this distribution. By
 * using this software in any fashion, you are agreeing to be bound by the
 * terms of this license.
 *
 * You must not remove this notice, or any other, from this software.
 */

#ifndef _DAT_SPRITES_H_
#define _DAT_SPRITES_H_

#include "xrick/data/sprites.h"

#ifdef GFXPC

enum { SPRITES_NBR_SPRITES = 155 };
extern sprite_t SPRITES_DATA[SPRITES_NBR_SPRITES];

#endif /* GFXPC */


#ifdef GFXST

enum { SPRITES_NBR_SPRITES = 213 };
extern sprite_t SPRITES_DATA[SPRITES_NBR_SPRITES];

#endif /* GFXST */

#endif /* ndef _DAT_SPRITES_H_ */

/* eof */

