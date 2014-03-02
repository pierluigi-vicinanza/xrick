/*
 * xrick/data/dat_tiles.h
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

#ifndef _DAT_TILES_H_
#define _DAT_TILES_H_

#include "tiles.h"

#ifdef GFXPC
enum { TILES_NBR_BANKS = 4 };
#endif

#ifdef GFXST
enum { TILES_NBR_BANKS = 3 };
#endif

extern tile_t TILES_DATA[TILES_NBR_BANKS][TILES_NBR_TILES];

#endif /* ndef _DAT_TILES_H_ */

/* eof */
