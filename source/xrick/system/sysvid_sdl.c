/*
 * xrick/system/sysvid_sdl.c
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

#include "xrick/control.h"
#include "xrick/draw.h"
#include "xrick/game.h"
#include "xrick/data/img.h"
#include "xrick/debug.h"
#include "xrick/system/system.h"

#include <string.h> /* memset */
#include <stdlib.h> /* malloc */
#include <SDL.h>

/*
 * Global variables
 */
U8 *sysvid_fb; /* frame buffer */

/*
 * Local variables
 */
static SDL_Color palette[256];
static SDL_Surface *screen;
static U32 videoFlags;
static bool isVideoInitialised = false;

static U8 zoom = SYSVID_ZOOM; /* actual zoom level */
static U8 szoom = 0;  /* saved zoom level */
static U8 fszoom = 0;  /* fullscreen zoom level */

#include "xrick/system/sdl_icon.e"

/*
 * Initialize screen
 */
static SDL_Surface *initScreen(U16 w, U16 h, U8 bpp, U32 flags)
{
    return SDL_SetVideoMode(w, h, bpp, flags);
}

/*
 *
 */
static void sysvid_restorePalette()
{
    SDL_SetColors(screen, (SDL_Color *)&palette, 0, 256);
}

/*
 *
 */
void sysvid_setPalette(img_color_t *pal, U16 n)
{
    U16 i;

    for (i = 0; i < n; i++)
    {
        palette[i].r = pal[i].r;
        palette[i].g = pal[i].g;
        palette[i].b = pal[i].b;
    }
    SDL_SetColors(screen, (SDL_Color *)&palette, 0, n);
}

/*
 *
 */
void sysvid_setGamePalette()
{
    sysvid_setPalette(game_colors, game_color_count);
}

/*
 * Initialize video modes
 */
static bool sysvid_chkvm(void)
{
  SDL_Rect **modes;
  U8 i, mode = 0;

  IFDEBUG_VIDEO(sys_printf("xrick/video: checking video modes\n"););

  modes = SDL_ListModes(NULL, videoFlags|SDL_FULLSCREEN);

  if (modes == (SDL_Rect **)0)
  {
    sys_error("(video) SDL can not find an appropriate video mode\n");
    return false;
  }

  if (modes == (SDL_Rect **)-1) {
    /* can do what you want, everything is possible */
    IFDEBUG_VIDEO(sys_printf("xrick/video: SDL says any video mode is OK\n"););
    fszoom = 1;
  }
  else {
    IFDEBUG_VIDEO(sys_printf("xrick/video: SDL says, use these modes:\n"););
    for (i = 0; modes[i]; i++) {
      IFDEBUG_VIDEO(sys_printf("  %dx%d\n", modes[i]->w, modes[i]->h););
      if (modes[i]->w <= modes[mode]->w && modes[i]->w >= SYSVID_WIDTH &&
      modes[i]->h * SYSVID_WIDTH >= modes[i]->w * SYSVID_HEIGHT) {
    mode = i;
    fszoom = modes[mode]->w / SYSVID_WIDTH;
      }
    }
    if (fszoom != 0) {
      IFDEBUG_VIDEO(
        sys_printf("xrick/video: fullscreen at %dx%d w/zoom=%d\n",
           modes[mode]->w, modes[mode]->h, fszoom);
    );
    }
    else {
      IFDEBUG_VIDEO(
        sys_printf("xrick/video: can not compute fullscreen zoom, use 1\n");
    );
      fszoom = 1;
    }
  }
  return true;
}

/*
 * Initialise video
 */
bool
sysvid_init(void)
{
    SDL_Surface *icon;
    U8 transpIndex, transpRed, transpGreen, transpBlue;
    U32 colorKey;
    /*
    U8 *mask;
    U32 len, i;
    */

    if (isVideoInitialised)
    {
        return true;
    }

    IFDEBUG_VIDEO(sys_printf("xrick/video: start\n"););

    /* SDL */
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) < 0)
    {
        sys_error("(video) could not init SDL");
        return false;
    }

    /* various Window Manager stuff */
    SDL_WM_SetCaption("xrick", "xrick");
    SDL_ShowCursor(SDL_DISABLE);

    icon = SDL_CreateRGBSurfaceFrom(IMG_ICON->pixels, IMG_ICON->width, IMG_ICON->height, 8, IMG_ICON->width, 0, 0, 0, 0);
    SDL_SetColors(icon, (SDL_Color *)IMG_ICON->colors, 0, IMG_ICON->ncolors);

    transpIndex = *(IMG_ICON->pixels);
    transpRed = IMG_ICON->colors[transpIndex].r;
    transpGreen = IMG_ICON->colors[transpIndex].r;
    transpBlue = IMG_ICON->colors[transpIndex].r;
    IFDEBUG_VIDEO(
        sys_printf("xrick/video: icon is %dx%d\n", IMG_ICON->width, IMG_ICON->height);
        sys_printf("xrick/video: icon transp. color is #%d (%d,%d,%d)\n",
        transpIndex, transpRed, transpGreen, transpBlue);
    );
    /*

    * old dirty stuff to implement transparency. SetColorKey does it
    * on Windows w/out problems. Linux? FIXME!

    len = IMG_ICON->w * IMG_ICON->h;
    mask = (U8 *)malloc(len/8);
    memset(mask, 0, len/8);
    for (i = 0; i < len; i++)
    if (IMG_ICON->pixels[i] != tpix) mask[i/8] |= (0x80 >> (i%8));
    */
    /*
    * FIXME
    * Setting a mask produces strange results depending on the
    * Window Manager. On fvwm2 it is shifted to the right ...
    */
    /*SDL_WM_SetIcon(s, mask);*/

    colorKey = SDL_MapRGB(icon->format, transpRed, transpGreen, transpBlue);
    SDL_SetColorKey(icon, SDL_SRCCOLORKEY, colorKey);

    SDL_WM_SetIcon(icon, NULL);

    /* video modes and screen */
    videoFlags = SDL_HWSURFACE|SDL_HWPALETTE;
    if (!sysvid_chkvm()) /* check video modes */
    {
        SDL_Quit();
        return false;
    }
    if (sysarg_args_zoom)
    {
        zoom = sysarg_args_zoom;
    }
    if (sysarg_args_fullscreen)
    {
        videoFlags |= SDL_FULLSCREEN;
        szoom = zoom;
        zoom = fszoom;
    }
    screen = initScreen(SYSVID_WIDTH * zoom, SYSVID_HEIGHT * zoom, 8, videoFlags);

    /*
    * create v_ frame buffer
    */
    sysvid_fb = malloc(SYSVID_WIDTH * SYSVID_HEIGHT);
    if (!sysvid_fb)
    {
        sys_error("(video) sysvid_fb malloc failed");
        SDL_Quit();
        return false;
    }

    isVideoInitialised = true;
    IFDEBUG_VIDEO(sys_printf("xrick/video: ready\n"););
    return true;
}

/*
 * Shutdown video
 */
void
sysvid_shutdown(void)
{
    if (!isVideoInitialised)
    {
        return;
    }

    free(sysvid_fb);
    SDL_Quit();
    isVideoInitialised = false;
    IFDEBUG_VIDEO(sys_printf("xrick/video: stop\n"););
}

/*
 * Update screen
 * NOTE errors processing ?
 */
void
sysvid_update(const rect_t *rects)
{
  static SDL_Rect area;
  U16 x, y, xz, yz;
  U8 *p, *q, *p0, *q0;

  if (rects == NULL)
    return;

  if (SDL_LockSurface(screen) == -1)
  {
    sys_error("(video): SDL_LockSurface failed");
    control_set(Control_EXIT);
    return;
  }

  while (rects) {
    p0 = sysvid_fb;
    p0 += rects->x + rects->y * SYSVID_WIDTH;
    q0 = (U8 *)screen->pixels;
    q0 += (rects->x + rects->y * SYSVID_WIDTH * zoom) * zoom;

    for (y = rects->y; y < rects->y + rects->height; y++) {
      for (yz = 0; yz < zoom; yz++) {
    p = p0;
    q = q0;
    for (x = rects->x; x < rects->x + rects->width; x++) {
      for (xz = 0; xz < zoom; xz++) {
        *q = *p;
        q++;
      }
      p++;
    }
    q0 += SYSVID_WIDTH * zoom;
      }
      p0 += SYSVID_WIDTH;
    }

    IFDEBUG_VIDEO2(
    for (y = rects->y; y < rects->y + rects->height; y++)
      for (yz = 0; yz < zoom; yz++) {
    p = (U8 *)screen->pixels + rects->x * zoom + (y * zoom + yz) * SYSVID_WIDTH * zoom;
    *p = 0x01;
    *(p + rects->width * zoom - 1) = 0x01;
      }

    for (x = rects->x; x < rects->x + rects->width; x++)
      for (xz = 0; xz < zoom; xz++) {
    p = (U8 *)screen->pixels + x * zoom + xz + rects->y * zoom * SYSVID_WIDTH * zoom;
    *p = 0x01;
    *(p + ((rects->height * zoom - 1) * zoom) * SYSVID_WIDTH) = 0x01;
      }
    );

    area.x = rects->x * zoom;
    area.y = rects->y * zoom;
    area.h = rects->height * zoom;
    area.w = rects->width * zoom;
    SDL_UpdateRects(screen, 1, &area);

    rects = rects->next;
  }

  SDL_UnlockSurface(screen);
}


/*
 * Clear screen
 * (077C)
 */
void
sysvid_clear(void)
{
  memset(sysvid_fb, 0, SYSVID_WIDTH * SYSVID_HEIGHT);
}


/*
 * Zoom
 */
void
sysvid_zoom(S8 z)
{
  if (!(videoFlags & SDL_FULLSCREEN) &&
      ((z < 0 && zoom > 1) ||
       (z > 0 && zoom < SYSVID_MAXZOOM))) {
    zoom += z;
    screen = initScreen(SYSVID_WIDTH * zoom,
            SYSVID_HEIGHT * zoom,
            screen->format->BitsPerPixel, videoFlags);
    sysvid_restorePalette();
    sysvid_update(&draw_SCREENRECT);
  }
}

/*
 * Toggle fullscreen
 */
void
sysvid_toggleFullscreen(void)
{
  videoFlags ^= SDL_FULLSCREEN;

  if (videoFlags & SDL_FULLSCREEN) {  /* go fullscreen */
    szoom = zoom;
    zoom = fszoom;
  }
  else {  /* go window */
    zoom = szoom;
  }
  screen = initScreen(SYSVID_WIDTH * zoom,
              SYSVID_HEIGHT * zoom,
              screen->format->BitsPerPixel, videoFlags);
  sysvid_restorePalette();
  sysvid_update(&draw_SCREENRECT);
}

/* eof */



