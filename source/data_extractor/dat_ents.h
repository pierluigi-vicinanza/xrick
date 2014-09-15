/*
 * data_extractor/dat_ents.h
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

#ifndef _DAT_ENTS_H
#define _DAT_ENTS_H

#include "xrick/ents.h"

enum { ENT_NBR_ENTDATA = 74 };
extern entdata_t ENT_ENTDATA[ENT_NBR_ENTDATA];

enum { ENT_NBR_SPRSEQ = 136 };
extern U8 ENT_SPRSEQ[ENT_NBR_SPRSEQ];

enum { ENT_NBR_MVSTEP = 784 };
extern mvstep_t ENT_MVSTEP[ENT_NBR_MVSTEP];

#endif /* ndef _DAT_ENTS_H */

/* eof */
