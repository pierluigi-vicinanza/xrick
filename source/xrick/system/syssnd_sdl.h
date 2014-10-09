/*
 * xrick/system/syssnd_sdl.h
 *
 * Copyright (C) 1998-2002 BigOrno (bigorno@bigorno.net).
 * Copyright (C) 2008-2014 Pierluigi Vicinanza.
 * All rights reserved.
 *
 * The use and distribution terms for this software are contained in the file
 * named README, which can be found in the root of this distribution. By
 * using this software in any fashion, you are agreeing to be bound by the
 * terms of this license.
 *
 * You must not remove this notice, or any other, from this software.
 */

#ifndef _SYSSND_H
#define _SYSSND_H

#include "xrick/config.h"

#ifdef ENABLE_SOUND

#include "xrick/data/sounds.h"

enum
{
    SYSSND_MAXVOL = 10,
    SYSSND_MIXCHANNELS = 8,
    /* MIXSAMPLES: 256 is too low on Windows. 512 means ~20 mix per second at 11025Hz */
    /* MIXSAMPLES: ?? at 22050Hz */
    SYSSND_MIXSAMPLES = 1024
};

typedef struct {
    sound_t *snd;
    U8 *buf;
    U32 len;
    S8 loop;
} channel_t;

extern void syssnd_load(sound_t *);
extern void syssnd_free(sound_t *);

#endif /* ENABLE_SOUND */

#endif /* ndef _SYSSND_H */

/* eof */


