/*
 * xrick/system/sysevt_sdl.c
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

/*
 * 20021010 SDLK_n replaced by SDLK_Fn because some non-US keyboards
 *          requires that SHIFT be pressed to input numbers.
 */

#include <SDL.h>

#include "xrick/system/system.h"
#include "xrick/config.h"
#include "xrick/game.h"
#include "xrick/debug.h"

#include "xrick/control.h"
#include "xrick/draw.h"

#define SYSJOY_RANGE 3280

static SDL_Event event;

/*
 * Process an event
 */
static void
processEvent()
{
    U16 key;
#ifdef ENABLE_FOCUS
    SDL_ActiveEvent *aevent;
#endif

  switch (event.type) {
  case SDL_KEYDOWN:
    key = event.key.keysym.sym;
    if (key == syskbd_up || key == SDLK_UP) {
      control_set(Control_UP);
    }
    else if (key == syskbd_down || key == SDLK_DOWN) {
      control_set(Control_DOWN);
    }
    else if (key == syskbd_left || key == SDLK_LEFT) {
      control_set(Control_LEFT);
    }
    else if (key == syskbd_right || key == SDLK_RIGHT) {
      control_set(Control_RIGHT);
    }
    else if (key == syskbd_pause) {
      control_set(Control_PAUSE);
    }
    else if (key == syskbd_end) {
      control_set(Control_END);
    }
    else if (key == syskbd_xtra) {
      control_set(Control_EXIT);
    }
    else if (key == syskbd_fire) {
      control_set(Control_FIRE);
    }
    else if (key == SDLK_F1) {
      sysvid_toggleFullscreen();
    }
    else if (key == SDLK_F2) {
      sysvid_zoom(-1);
    }
    else if (key == SDLK_F3) {
      sysvid_zoom(+1);
    }
#ifdef ENABLE_SOUND
    else if (key == SDLK_F4) {
      syssnd_toggleMute();
    }
    else if (key == SDLK_F5) {
      syssnd_vol(-1);
    }
    else if (key == SDLK_F6) {
      syssnd_vol(+1);
    }
#endif
#ifdef ENABLE_CHEATS
    else if (key == SDLK_F7) {
      game_toggleCheat(Cheat_UNLIMITED_ALL);
    }
    else if (key == SDLK_F8) {
      game_toggleCheat(Cheat_NEVER_DIE);
    }
    else if (key == SDLK_F9) {
      game_toggleCheat(Cheat_EXPOSE);
    }
#endif
    break;
  case SDL_KEYUP:
    key = event.key.keysym.sym;
    if (key == syskbd_up || key == SDLK_UP) {
      control_clear(Control_UP);
    }
    else if (key == syskbd_down || key == SDLK_DOWN) {
      control_clear(Control_DOWN);
    }
    else if (key == syskbd_left || key == SDLK_LEFT) {
      control_clear(Control_LEFT);
    }
    else if (key == syskbd_right || key == SDLK_RIGHT) {
      control_clear(Control_RIGHT);
    }
    else if (key == syskbd_pause) {
      control_clear(Control_PAUSE);
    }
    else if (key == syskbd_end) {
      control_clear(Control_END);
    }
    else if (key == syskbd_xtra) {
      control_clear(Control_EXIT);
    }
    else if (key == syskbd_fire) {
      control_clear(Control_FIRE);
    }
    break;
  case SDL_QUIT:
    /* player tries to close the window -- this is the same as pressing ESC */
    control_set(Control_EXIT);
    break;
#ifdef ENABLE_FOCUS
  case SDL_ACTIVEEVENT: {
    aevent = (SDL_ActiveEvent *)&event;
    IFDEBUG_EVENTS(
      printf("xrick/events: active %x %x\n", aevent->gain, aevent->state);
      );
    if (aevent->gain == 1)
      control_active = true;
    else
      control_active = false;
    }
  break;
#endif
#ifdef ENABLE_JOYSTICK
  case SDL_JOYAXISMOTION:
    IFDEBUG_EVENTS(sys_printf("xrick/events: joystick\n"););
    if (event.jaxis.axis == 0) {  /* left-right */
      if (event.jaxis.value < -SYSJOY_RANGE) {  /* left */
    control_set(Control_LEFT);
    control_clear(Control_RIGHT);
      }
      else if (event.jaxis.value > SYSJOY_RANGE) {  /* right */
    control_set(Control_RIGHT);
    control_clear(Control_LEFT);
      }
      else {  /* center */
    control_clear(Control_RIGHT);
    control_clear(Control_LEFT);
      }
    }
    if (event.jaxis.axis == 1) {  /* up-down */
      if (event.jaxis.value < -SYSJOY_RANGE) {  /* up */
    control_set(Control_UP);
    control_clear(Control_DOWN);
      }
      else if (event.jaxis.value > SYSJOY_RANGE) {  /* down */
    control_set(Control_DOWN);
    control_clear(Control_UP);
      }
      else {  /* center */
    control_clear(Control_DOWN);
    control_clear(Control_UP);
      }
    }
    break;
  case SDL_JOYBUTTONDOWN:
    control_set(Control_FIRE);
    break;
  case SDL_JOYBUTTONUP:
    control_clear(Control_FIRE);
    break;
#endif
  default:
    break;
  }
}

/*
 * Process events, if any, then return
 */
void
sysevt_poll(void)
{
  while (SDL_PollEvent(&event))
    processEvent();
}

/*
 * Wait for an event, then process it and return
 */
void
sysevt_wait(void)
{
  SDL_WaitEvent(&event);
  processEvent();
}

/* eof */



