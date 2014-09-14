/*
 * xrick/data/dat_maps.h
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

#ifndef _DAT_MAPS_H
#define _DAT_MAPS_H

#include "xrick/maps.h"

enum { MAP_NBR_MAPS = 5 };
extern map_t MAP_MAPS[MAP_NBR_MAPS];

enum { MAP_NBR_SUBMAPS = 47 };
extern submap_t MAP_SUBMAPS[MAP_NBR_SUBMAPS];

enum { MAP_NBR_CONNECT = 153 };
extern connect_t MAP_CONNECT[MAP_NBR_CONNECT];

enum { MAP_NBR_BLOCKS = 0x0100 };
extern block_t MAP_BLOCKS[MAP_NBR_BLOCKS];

enum { MAP_NBR_MARKS = 523 };
extern mark_t MAP_MARKS[MAP_NBR_MARKS];

enum { MAP_NBR_BNUMS = 8152 };
extern U8 MAP_BNUMS[MAP_NBR_BNUMS];

enum { MAP_NBR_EFLGC = 0x0020 };
extern U8 MAP_EFLG_C[MAP_NBR_EFLGC];

#endif /* ndef _DAT_MAPS_H */

/* eof */
