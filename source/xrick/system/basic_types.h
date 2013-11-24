/*
 * system/basic_types.h
 *
 * Copyright (C) 1998-2002 BigOrno (bigorno@bigorno.net). All rights reserved.
 *
 * The use and distribution terms for this software are contained in the file
 * named README, which can be found in the root of this distribution. By
 * using this software in any fashion, you are agreeing to be bound by the
 * terms of this license.
 *
 * You must not remove this notice, or any other, from this software.
 */

#ifndef _BASIC_TYPES_H
#define _BASIC_TYPES_H

#ifdef _MSC_VER

typedef enum { false, true } bool;

#define inline __inline

typedef unsigned __int8  U8;
typedef unsigned __int16 U16;
typedef unsigned __int32 U32;
typedef          __int8  S8;
typedef          __int16 S16;
typedef          __int32 S32;

#else /* ndef _MSC_VER */

#include <stdbool.h>

#include <stdint.h>
typedef uint8_t U8;         /*  8 bits unsigned */
typedef uint16_t U16;   /* 16 bits unsigned */
typedef uint32_t U32;         /* 32 bits unsigned */
typedef int8_t S8;           /*  8 bits signed   */
typedef int16_t S16;     /* 16 bits signed   */
typedef int32_t S32;           /* 32 bits signed   */

#endif /* def _MSC_VER */

#endif

/* eof */


