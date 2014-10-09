/*
 * xrick/system/sysjoy_sdl.c
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

#include "xrick/config.h"

#ifdef ENABLE_JOYSTICK

#include "xrick/system/system.h"
#include "xrick/debug.h"

#include <SDL.h>

static SDL_Joystick *joystick = NULL;
static bool isJoystickInitialised = false;

bool
sysjoy_init(void)
{
    int deviceCount;

    if (isJoystickInitialised)
    {
        return true;
    }

    IFDEBUG_JOYSTICK(sys_printf("xrick/joystick: start\n"););

    if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0)
    {
        IFDEBUG_JOYSTICK(
            sys_printf("xrick/joystick: can not initialize joystick subsystem\n");
        );
        return true; /* shall we treat this as an error? */
    }

    deviceCount = SDL_NumJoysticks();
    if (deviceCount > 0)
    {
        /* use the first joystick that we can open */
        int deviceIndex;
        for (deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++)
        {
            joystick = SDL_JoystickOpen(deviceIndex);
            if (joystick)
            {
                SDL_JoystickEventState(SDL_ENABLE); /* enable events */
                break;
            }
        }
    }
    else /* no joystick on this system */
    {
        IFDEBUG_JOYSTICK(sys_printf("xrick/joystick: no joystick available\n"););
    }

    isJoystickInitialised = true;
    IFDEBUG_JOYSTICK(sys_printf("xrick/joystick: ready\n"););
    return true;
}

void
sysjoy_shutdown(void)
{
    if (!isJoystickInitialised)
    {
        return;
    }

    if (joystick)
    {
        SDL_JoystickClose(joystick);
    }
    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
    isJoystickInitialised = false;
    IFDEBUG_JOYSTICK(sys_printf("xrick/joystick: stop\n"););
}

#endif /* ENABLE_JOYSTICK */

/* eof */

