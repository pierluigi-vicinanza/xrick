/*
 * xrick/src/sysarg.c
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

/*
 * 20021010 added test to prevent buffer overrun in -keys parsing.
 */

#include <stdlib.h>  /* atoi */
#include <string.h>  /* strcasecmp */

#include <SDL.h>

#include "xrick/system/system.h"
#include "xrick/config.h"
#include "xrick/game.h"

#include "xrick/maps.h"
#include "xrick/system/syssnd.h"

/* handle Microsoft Visual C */
#ifdef _MSC_VER
#define strcasecmp stricmp
#endif

typedef struct {
  char name[16];
  int code;
} sdlcodes_t;

static sdlcodes_t sdlcodes[SDLK_LAST] = {
#include "xrick/system/sdlcodes.e"
};

int sysarg_args_period = 0;
int sysarg_args_map = 0;
int sysarg_args_submap = 0;
int sysarg_args_fullscreen = 0;
int sysarg_args_zoom = 0;
bool sysarg_args_nosound = false;
int sysarg_args_vol = 0;
const char *sysarg_args_data = NULL;

/*
 * Fail
 */
static void sysarg_fail(char *msg)
{
#ifdef ENABLE_SOUND
	printf("xrick [version #%s]: %s\n"
           "usage: xrick [<options>]\n"
           "<option> =\n"
           "  -h, -help : Display this information.\n"
           "  -fullscreen : Run in fullscreen mode.\n"
           "    The default is to run in a window.\n"
           "  -speed <speed> : Run at speed <speed>. Speed must be an integer between 1\n"
           "    (fast) and 100 (slow). The default is %d\n"
           "  -zoom <zoom> : Display with zoom factor <zoom>. <zoom> must be an integer\n"
           "    between 1 (320x200) and %d (%d times bigger). The default is 2.\n"
           "  -map <map> : Start at map number <map>. <map> must be an integer between\n"
           "    1 and %d. The default is to start at map number 1\n"
           "  -submap <submap> : Start at submap <submap>. <submap> must be an integer\n"
           "    between 1 and %d. The default is to start at submap number 1 or, if a map\n"
           "    was specified, at the first submap of that map.\n"
           "  -keys <left>-<right>-<up>-<down>-<fire> : Override the default key\n"
           "    bindings (cf. KeyCodes)\n"
           "  -nosound : Disable sounds. The default is to play with sounds enabled.\n"
           "  -vol <vol> : Play sounds at volume <vol>. <vol> must be an integer\n"
           "    between 0 (silence) and %d (max). The default is to play sounds\n"
           "    at maximal volume (%d).\n", VERSION, msg, GAME_PERIOD, SYSVID_MAXZOOM, SYSVID_MAXZOOM, 5/*MAP_NBR_MAPS*/-1, 47/*MAP_NBR_SUBMAPS*/, SYSSND_MAXVOL, SYSSND_MAXVOL);
#else
	printf("xrick [version #%s]: %s\n"
           "usage: xrick [<options>]\n"
           "<option> =\n"
           "  -h, -help : Display this information.\n"
           "  -fullscreen : Run in fullscreen mode.\n"
           "    The default is to run in a window.\n"
           "  -speed <speed> : Run at speed <speed>. Speed must be an integer between 1\n"
           "    (fast) and 100 (slow). The default is %d\n"
           "  -zoom <zoom> : Display with zoom factor <zoom>. <zoom> must be an integer\n"
           "    between 1 (320x200) and %d (%d times bigger). The default is 2.\n"
           "  -map <map> : Start at map number <map>. <map> must be an integer between\n"
           "    1 and %d. The default is to start at map number 1\n"
           "  -submap <submap> : Start at submap <submap>. <submap> must be an integer\n"
           "    between 1 and %d. The default is to start at submap number 1 or, if a map\n"
           "    was specified, at the first submap of that map.\n"
           "  -keys <left>-<right>-<up>-<down>-<fire> : Override the default key\n"
           "    bindings (cf. KeyCodes)\n", VERSION, msg, GAME_PERIOD, SYSVID_MAXZOOM, SYSVID_MAXZOOM, 5/*MAP_NBR_MAPS*/-1, 47/*MAP_NBR_SUBMAPS*/);
#endif
    /* TODO: remove hardcoded map/submap max counts because they are now loaded from resource files */
}

/*
 * Get SDL key code
 */
static int sysarg_sdlcode(char *k)
{
  int i, result;

  i = 0;
  result = 0;

  while (sdlcodes[i].code) {
    if (!strcasecmp(sdlcodes[i].name, k)) {
      result = sdlcodes[i].code;
      break;
    }
    i++;
  }

  return result;
}

/*
 * Scan key codes sequence
 */
static bool sysarg_scankeys(const char *keys)
{
  char k[16];
  int i, j;

  i = 0;

  j = 0;
  while (keys[i] != '\0' && keys[i] != '-' && j + 1 < sizeof k) k[j++] = keys[i++];
  if (keys[i++] == '\0') return false;
  k[j] = '\0';
  syskbd_left = sysarg_sdlcode(k);
  if (!syskbd_left) return false;

  j = 0;
  while (keys[i] != '\0' && keys[i] != '-' && j + 1 < sizeof k) k[j++] = keys[i++];
  if (keys[i++] == '\0') return false;
  k[j] = '\0';
  syskbd_right = sysarg_sdlcode(k);
  if (!syskbd_right) return false;

  j = 0;
  while (keys[i] != '\0' && keys[i] != '-' && j + 1 < sizeof k) k[j++] = keys[i++];
  if (keys[i++] == '\0') return false;
  k[j] = '\0';
  syskbd_up = sysarg_sdlcode(k);
  if (!syskbd_up) return false;

  j = 0;
  while (keys[i] != '\0' && keys[i] != '-' && j + 1 < sizeof k) k[j++] = keys[i++];
  if (keys[i++] == '\0') return false;
  k[j] = '\0';
  syskbd_down = sysarg_sdlcode(k);
  if (!syskbd_down) return false;

  j = 0;
  while (keys[i] != '\0' && keys[i] != '-' && j + 1 < sizeof k) k[j++] = keys[i++];
  if (keys[i] != '\0') return false;
  k[j] = '\0';
  syskbd_fire = sysarg_sdlcode(k);
  if (!syskbd_fire) return false;

  return true;
}

/*
 * Read and process arguments
 */
bool
sysarg_init(int argc, const char **argv)
{
    int i;

    for (i = 1; i < argc; i++) 
    {
        if (!strcmp(argv[i], "-fullscreen")) 
        {
            sysarg_args_fullscreen = 1;
        }
        else if (!strcmp(argv[i], "-help") ||
                 !strcmp(argv[i], "-h")) 
        {
            sysarg_fail("help");
            return false;
        }
        else if (!strcmp(argv[i], "-speed")) 
        {
            if (++i == argc) 
            {
                sysarg_fail("missing speed value");
                return false;
            }
            sysarg_args_period = atoi(argv[i]) - 1;
            if (sysarg_args_period < 0 || sysarg_args_period > 99)
            {
                sysarg_fail("invalid speed value");
                return false;
            }
        }
        else if (!strcmp(argv[i], "-keys")) 
        {
            if (++i == argc) 
            {
                sysarg_fail("missing key codes");
                return false;
            }
            if (!sysarg_scankeys(argv[i]))
            {
                sysarg_fail("invalid key codes");
                return false;
            }
        }
        else if (!strcmp(argv[i], "-zoom")) 
        {
            if (++i == argc) 
            {
                sysarg_fail("missing zoom value");
                return false;
            }
            sysarg_args_zoom = atoi(argv[i]);
            if (sysarg_args_zoom < 1 || sysarg_args_zoom > SYSVID_MAXZOOM)
            {
                sysarg_fail("invalid zoom value");
                return false;
            }
        }
        else if (!strcmp(argv[i], "-map")) 
        {
            if (++i == argc) 
            {
                sysarg_fail("missing map number");
                return false;
            }
            sysarg_args_map = atoi(argv[i]) - 1;
            if (sysarg_args_map < 0 || sysarg_args_map >= 5/*MAP_NBR_MAPS*/-1) /* TODO: remove hardcoded map max count */
            {
                sysarg_fail("invalid map number");
                return false;
            }
        }
        else if (!strcmp(argv[i], "-submap")) 
        {
            if (++i == argc) 
            {
                sysarg_fail("missing submap number");
                return false;
            }
            sysarg_args_submap = atoi(argv[i]) - 1;
            if (sysarg_args_submap < 0 || sysarg_args_submap >= 47/*MAP_NBR_SUBMAPS*/) /* TODO: remove hardcoded submap max count */
            {
                sysarg_fail("invalid submap number");
                return false;
            }
        }
#ifdef ENABLE_SOUND
        else if (!strcmp(argv[i], "-vol")) 
        {
            if (++i == argc) 
            {
                sysarg_fail("missing volume");
                return false;
            }
            sysarg_args_vol = atoi(argv[i]) - 1;
            if (sysarg_args_submap < 0 || sysarg_args_submap >= SYSSND_MAXVOL)
            {
                sysarg_fail("invalid volume");
                return false;
            }
        }
        else if (!strcmp(argv[i], "-nosound")) 
        {
            sysarg_args_nosound = true;
        }
#endif /* ENABLE_SOUND */
        else if (!strcmp(argv[i], "-data")) 
        {
            if (++i == argc) 
            {
                sysarg_fail("missing data");
                return false;
            }
            sysarg_args_data = argv[i];
        }
        else 
        {
            sysarg_fail("invalid argument(s)");
            return false;
        }
    }

    /* TODO: remove checks below based on hardcoded values. 
    *       Add code to check sysarg_args_map and sysarg_args_submap against map/submap max counts 
    *       (after these have been loaded from resource files). 
    */

    /* this is dirty (sort of) */
    if (sysarg_args_submap > 0 && sysarg_args_submap < 9)
    {
        sysarg_args_map = 0;
    }
    if (sysarg_args_submap >= 9 && sysarg_args_submap < 20)
    {
        sysarg_args_map = 1;
    }
    if (sysarg_args_submap >= 20 && sysarg_args_submap < 38)
    {
        sysarg_args_map = 2;
    }
    if (sysarg_args_submap >= 38)
    {
        sysarg_args_map = 3;
    }
    if (sysarg_args_submap == 9 ||
        sysarg_args_submap == 20 ||
        sysarg_args_submap == 38)
    {
        sysarg_args_submap = 0;
    }
    return true;
}

/* eof */





