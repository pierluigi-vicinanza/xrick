/*
 * xrick/src/syssnd.c
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

#include <SDL.h>
#include <stdlib.h>
#include <memory.h>

#include "xrick/config.h"

#ifdef ENABLE_SOUND

#include "xrick/system/system.h"
#include "xrick/game.h"
#include "xrick/system/syssnd.h"
#include "xrick/debug.h"

const U8 syssnd_period = 0xff; /* not needed under current SDL implementation of xrick */

#define ADJVOL(S) (((S)*sndVol)/SDL_MIX_MAXVOLUME)

static bool isAudioInitialised = false;
static channel_t channel[SYSSND_MIXCHANNELS];

static U8 sndVol = SDL_MIX_MAXVOLUME;  /* internal volume */
static U8 sndUVol = SYSSND_MAXVOL;  /* user-selected volume */
static bool sndMute = false;  /* mute flag */

/*
 * prototypes
 */
static void sdl_callback(void *userdata, U8 *stream, int len);
static int sdlRWops_open(SDL_RWops *context, char *name);
static int sdlRWops_seek(SDL_RWops *context, int offset, int whence);
static int sdlRWops_read(SDL_RWops *context, void *ptr, int size, int maxnum);
static int sdlRWops_write(SDL_RWops *context, const void *ptr, int size, int num);
static int sdlRWops_close(SDL_RWops *context);
static void endChannel(size_t);

/*
 * Callback -- this is also where all sound mixing is done
 *
 * Note: it may not be that much a good idea to do all the mixing here ; it
 * may be more efficient to mix samples every frame, or maybe everytime a
 * new sound is sent to be played. I don't know.
 */
static void sdl_callback(UNUSED(void *userdata), U8 *stream, int len)
{
    int i;
    for (i = 0; i < len; i++) 
    {
        size_t c;
        S16 s = 0;

        for (c = 0; c < SYSSND_MIXCHANNELS; c++) 
        {
            if (channel[c].loop != 0) 
            {  /* channel is active */
                if (channel[c].len > 0) 
                {  /* not ending */
                    s += ADJVOL(*channel[c].buf - 0x80);
                    channel[c].buf++;
                    channel[c].len--;
                }
                else 
                {  /* ending */
                    if (channel[c].loop > 0) channel[c].loop--;
                    if (channel[c].loop) 
                    {  /* just loop */
                        IFDEBUG_AUDIO2(sys_printf("xrick/audio: channel %d - loop\n", c););
                        channel[c].buf = channel[c].snd->buf;
                        channel[c].len = channel[c].snd->len;
                        s += ADJVOL(*channel[c].buf - 0x80);
                        channel[c].buf++;
                        channel[c].len--;
                    }
                    else 
                    {  /* end for real */
                        IFDEBUG_AUDIO2(sys_printf("xrick/audio: channel %d - end\n", c););
                        endChannel(c);
                    }
                }
            }
        }
        if (sndMute)
        {
            stream[i] = 0x80;
        }
        else 
        {
            s += 0x80;
            if (s > 0xff) s = 0xff;
            if (s < 0x00) s = 0x00;
            stream[i] = (U8)s;
        }
    }
}

/*
 * Deactivate channel and unload associated resources
 */
static void endChannel(size_t c)
{
	channel[c].loop = 0;
	if (channel[c].snd->dispose)
    {
		syssnd_free(channel[c].snd);
    }
    channel[c].snd = NULL;
}

/*
 * Initialise audio
 */
bool syssnd_init(void)
{
    SDL_AudioSpec desired, obtained;
    size_t c;

    if (isAudioInitialised)
    {
        return true;
    }

    IFDEBUG_AUDIO(sys_printf("xrick/audio: start\n"););

    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) 
    {
        sys_error("(audio) can not initialize audio subsystem");
        return true; /* shall we treat this as an error? */
    }

    desired.freq = Wave_SAMPLE_RATE;
    desired.format = AUDIO_U8;
    desired.channels = Wave_CHANNEL_COUNT;
    desired.samples = SYSSND_MIXSAMPLES;
    desired.callback = sdl_callback;
    desired.userdata = NULL;

    if (SDL_OpenAudio(&desired, &obtained) < 0) 
    {
        sys_error("(audio) can not open audio (%s)", SDL_GetError());
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return true; /* shall we treat this as an error? */
    }

    if (sysarg_args_vol != 0) 
    {
        sndUVol = sysarg_args_vol;
        sndVol = SDL_MIX_MAXVOLUME * sndUVol / SYSSND_MAXVOL;
    }

    for (c = 0; c < SYSSND_MIXCHANNELS; c++)
    {
        channel[c].loop = 0;  /* deactivate */
    }

    SDL_PauseAudio(0);
    isAudioInitialised = true;
    IFDEBUG_AUDIO(sys_printf("xrick/audio: ready\n"););
    return true;
}

/*
 * Shutdown
 */
void syssnd_shutdown(void)
{
    if (!isAudioInitialised) 
    {
        return;
    }

    SDL_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    isAudioInitialised = false;
    IFDEBUG_AUDIO(sys_printf("xrick/audio: stop\n"););
}

/*
 * Toggle mute
 *
 * When muted, sounds are still managed but not sent to the dsp, hence
 * it is possible to un-mute at any time.
 */
void syssnd_toggleMute(void)
{
    if (!isAudioInitialised) 
    {
        return;
    }

    SDL_LockAudio();
    sndMute = !sndMute;
    SDL_UnlockAudio();
}

/*
 * Increase/decrease volume
 */
void syssnd_vol(S8 d)
{
    if (!isAudioInitialised) 
    {
        return;
    }

    if ((d < 0 && sndUVol > 0) ||
        (d > 0 && sndUVol < SYSSND_MAXVOL)) 
    {
        sndUVol += d;
        SDL_LockAudio();
        sndVol = SDL_MIX_MAXVOLUME * sndUVol / SYSSND_MAXVOL;
        SDL_UnlockAudio();
    }
}

/*
 * Play a sound
 *
 * loop: number of times the sound should be played, -1 to loop forever
 *
 * NOTE if sound is already playing, simply reset it (i.e. can not have
 * twice the same sound playing -- tends to become noisy when too many
 * bad guys die at the same time).
 */
void syssnd_play(sound_t *sound, S8 loop)
{
    S8 c;

    if (!isAudioInitialised || !sound)
    {
        return;
    }

    {
        SDL_LockAudio();
        
        c = 0;
        while (channel[c].snd != sound &&
            channel[c].loop != 0 &&
            c < SYSSND_MIXCHANNELS)
        {
            c++;
        }

        SDL_UnlockAudio();
    }

    if (c >= SYSSND_MIXCHANNELS)
    {
        return;
    }

    if (!sound->buf)
    {
        syssnd_load(sound);
        if (!sound->buf)
        {
            sys_error("(audio) can not load %s\n", sound->name);
            return;
        }
    }

    {
        SDL_LockAudio();

        IFDEBUG_AUDIO(
            if (channel[c].snd == sound)
            {
                sys_printf("xrick/audio: already playing %s on channel %d - resetting\n",
                    sound->name, c);
            }
            else
            {
                sys_printf("xrick/audio: playing %s on channel %d\n", sound->name, c);
            }
        );

        channel[c].loop = loop;
        channel[c].snd = sound;
        channel[c].buf = sound->buf;
        channel[c].len = sound->len;

        SDL_UnlockAudio();
    }
}

/*
 * Pause all sounds
 */
void syssnd_pauseAll(bool pause)
{
    if (!isAudioInitialised) 
    {
        return;
    }

    SDL_PauseAudio(pause);
}

/*
 * Stop a sound
 */
void syssnd_stop(sound_t *sound)
{
	size_t i;

    if (!isAudioInitialised || !sound)
    {
        return;
    }

	SDL_LockAudio();
	for (i = 0; i < SYSSND_MIXCHANNELS; i++)
    {
        if (channel[i].snd == sound) 
        {
            endChannel(i);
        }
    }
	SDL_UnlockAudio();
}

/*
 * Stops all channels.
 */
void syssnd_stopAll(void)
{
	size_t i;

    if (!isAudioInitialised) 
    {
        return;
    }

	SDL_LockAudio();
	for (i = 0; i < SYSSND_MIXCHANNELS; i++)
    {
		if (channel[i].snd)
        {
            endChannel(i);
        }
    }
	SDL_UnlockAudio();
}

/*
 * Load a sound.
 */
void syssnd_load(sound_t *sound)
{
	SDL_RWops *context;
	SDL_AudioSpec audiospec;
    bool success;

    if (!isAudioInitialised || !sound)
    {
        return;
    }

	/* alloc context */
	context = malloc(sizeof(SDL_RWops));
	context->seek = sdlRWops_seek;
	context->read = sdlRWops_read;
	context->write = sdlRWops_write;
	context->close = sdlRWops_close;

    success = false;
    do
    {
        /* open */
        if (sdlRWops_open(context, sound->name) == -1)
        {
            free(context);
            break;
        }

        /* read */
        /* second param == 1 -> close source once read (context will be freed on close)*/
        if (!SDL_LoadWAV_RW(context, 1, &audiospec, &(sound->buf), &(sound->len)))
        {
            break;
        }

        success = true;
    } while (false);

    if (!success)
    {
        sound->buf = NULL;
        sound->len = 0;
    }
}

/*
 * Unload a sound
 */
void syssnd_free(sound_t *sound)
{  
    if (!isAudioInitialised || !sound || !sound->buf) 
    {
        return;
    }

    SDL_FreeWAV(sound->buf);
	sound->buf = NULL;
	sound->len = 0;
}

/*
 * Mix audio samples and fill playback buffer
 * 
 * Note: all work is currently done in "sdl_callback(...)". This might change in future.
 */
void syssnd_update(void)
{
}

/*
 *
 */
static int
sdlRWops_open(SDL_RWops *context, char *name)
{
	file_t f;

	f = sysfile_open(name);
	if (!f) return -1;
	context->hidden.unknown.data1 = (void *)f;

	return 0;
}

static int
sdlRWops_seek(SDL_RWops *context, int offset, int whence)
{
	return sysfile_seek((file_t)(context->hidden.unknown.data1), offset, whence);
}

static int
sdlRWops_read(SDL_RWops *context, void *ptr, int size, int maxnum)
{
	return sysfile_read((file_t)(context->hidden.unknown.data1), ptr, size, maxnum);
}

static int
sdlRWops_write(SDL_RWops *context, const void *ptr, int size, int num)
{
	/* not implemented */
	return -1;
}

static int
sdlRWops_close(SDL_RWops *context)
{
    sysfile_close((file_t)(context->hidden.unknown.data1));
	return 0;
}

#endif /* ENABLE_SOUND */

/* eof */

