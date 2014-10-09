/*
 * xrick/system/sysarg_sdl.c
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
 * 20021010 added test to prevent buffer overrun in -keys parsing.
 */

#include "xrick/system/system.h"
#include "xrick/config.h"
#include "xrick/game.h"

#include "xrick/maps.h"
#include "xrick/system/syssnd_sdl.h"

#include <stdlib.h>  /* atoi */
#include <string.h>  /* strcasecmp */

#include <SDL.h>

/* handle Microsoft Visual C */
#ifdef _MSC_VER
#define strcasecmp _stricmp
#endif

typedef struct {
  char name[16];
  int code;
} sdlcodes_t;

static sdlcodes_t sdlcodes[SDLK_LAST] = {
#include "xrick/system/sdl_codes.e"
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
 * Version info
 */
static void sysarg_version(void)
{
    sys_printf(
        "xrick version '%s'\n\n"
        " Copyright (C) 1998-2002 BigOrno (bigorno@bigorno.net).\n"
        " Copyright (C) 2008-2014 Pierluigi Vicinanza.\n"
        " All rights reserved.\n\n"
        " The use and distribution terms for this software are contained in the file\n"
        " named README, which can be found in the root of this distribution. By\n"
        " using this software in any fashion, you are agreeing to be bound by the\n"
        " terms of this license.\n\n", XRICK_VERSION_STR);
}

/*
 * Help
 */
static void sysarg_help(void)
{
   sys_printf(
       "Usage: xrick [option(s)]\n"
       " The options are:\n\n"
       "  -h, --help         Display this information\n"
       "  --fullscreen       Run in fullscreen mode.\n"
       "                     The default is to run in a window.\n"
       "  --speed <speed>    Run at speed <speed>. <speed> must be \n"
       "                     an integer between 1 (fast) and 100 (slow).\n"
       "                     The default is %d.\n"
       "  --zoom <zoom>      Display with zoom factor <zoom>.\n"
       "                     <zoom> must be an integer between 1 (320x200)\n"
       "                     and %d (%d times bigger). The default is %d.\n"
       "  --map <map>        Start at map number <map>.\n"
       "                     <map> must be an integer between 1 and %d.\n"
       "                     The default is to start at map number 1.\n"
       "  --submap <submap>  Start at submap <submap>.\n"
       "                     <submap> must be an integer between 1 and %d.\n"
       "                     The default is to start at submap number 1\n"
       "                     or, if a map was specified,\n"
       "                     at the first submap of that map.\n"
       "  --keys <left>-<right>-<up>-<down>-<fire>\n"
       "                     Override the default key bindings\n"
       "                     (cf. KeyCodes).\n"
       "  --data <archive>   Use data archive <archive>\n"
       "                     <archive> must be either a zip file or\n"
       "                     a directory. The default is to look for \"data.zip\"\n"
       "                     in the directory where xrick is run from.\n"
#ifdef ENABLE_SOUND
       "  --nosound          Disable sounds.\n"
       "                     The default is to play with sounds enabled.\n"
       "  --vol <vol>        Play sounds at volume <vol>.\n"
       "                     <vol> must be an integer between 0 (silence)\n"
       "                     and %d (max). The default is to play sounds\n"
       "                     at maximum volume (%d).\n"
#endif /* ENABLE_SOUND */
       "  --version          Print version information.\n\n",
       GAME_PERIOD, SYSVID_MAXZOOM, SYSVID_MAXZOOM, SYSVID_ZOOM, 5/*MAP_NBR_MAPS*/-1, 47/*MAP_NBR_SUBMAPS*/
#ifdef ENABLE_SOUND
       , SYSSND_MAXVOL, SYSSND_MAXVOL
#endif /* ENABLE_SOUND */
       );
   /* TODO: remove hardcoded map/submap max counts because they are now loaded from resource files */
}

/*
 * Fail
 */
static void sysarg_fail(char *msg)
{
    sys_printf(
        "xrick: %s\n"
        " Use 'xrick --help' for a complete list of options.\n", msg);
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
sysarg_init(int argc, char **argv)
{
    int i;

    for (i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "--fullscreen"))
        {
            sysarg_args_fullscreen = 1;
        }
        else if (!strcmp(argv[i], "--help") ||
                 !strcmp(argv[i], "-h"))
        {
            sysarg_help();
            return false;
        }
        else if (!strcmp(argv[i], "--speed"))
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
        else if (!strcmp(argv[i], "--keys"))
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
        else if (!strcmp(argv[i], "--zoom"))
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
        else if (!strcmp(argv[i], "--map"))
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
        else if (!strcmp(argv[i], "--submap"))
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
        else if (!strcmp(argv[i], "--vol"))
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
        else if (!strcmp(argv[i], "--nosound"))
        {
            sysarg_args_nosound = true;
        }
#endif /* ENABLE_SOUND */
        else if (!strcmp(argv[i], "--data"))
        {
            if (++i == argc)
            {
                sysarg_fail("missing data");
                return false;
            }
            sysarg_args_data = argv[i];
        }
        else if (!strcmp(argv[i], "--version"))
        {
            sysarg_version();
            return false;
        }
        else
        {
            char message[128];
            sys_snprintf(message, sizeof(message), "unrecognized option '%s'", argv[i]);
            sysarg_fail(message);
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





