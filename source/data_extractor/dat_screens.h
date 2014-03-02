/*
 * xrick/data/dat_screens.h
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

#ifndef _DAT_SCREENS_H
#define _DAT_SCREENS_H

#include "screens.h"

enum { SCREEN_NBR_IMAPSL = 22 };
extern U8 SCREEN_IMAPSL[SCREEN_NBR_IMAPSL];  /* sprite lists */

enum { SCREEN_NBR_IMAPSTESPS = 23 };
extern screen_imapsteps_t SCREEN_IMAPSTEPS[SCREEN_NBR_IMAPSTESPS];  /* map intro steps */

enum { SCREEN_NBR_IMAPSOFS = 5 };
extern U8 SCREEN_IMAPSOFS[SCREEN_NBR_IMAPSOFS];  /* first step for each map */

enum { SCREEN_NBR_IMAPTEXT = 5 };
extern U8 *SCREEN_IMAPTEXT[SCREEN_NBR_IMAPTEXT];  /* map intro texts */

enum { SCREEN_NBR_HISCORES = 8 };
extern hiscore_t SCREEN_HIGHSCORES[SCREEN_NBR_HISCORES];  /* highest scores (hall of fame) */

#ifdef GFXPC
extern U8 SCREEN_IMAINHOFT[];  /* hall of fame title */
extern U8 SCREEN_IMAINRDT[];  /* rick dangerous title */
extern U8 SCREEN_IMAINCDC[];  /* core design copyright text */
extern U8 SCREEN_CONGRATS[];  /* congratulations */
#endif
extern U8 SCREEN_GAMEOVERTXT[];  /* game over */
extern U8 SCREEN_PAUSEDTXT[];  /* paused */

#endif /* ndef _DAT_SCREENS_H */

/* eof */
