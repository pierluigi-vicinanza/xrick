/*
 * data_extractor/main.c
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

#include "config.h"
#include "resources.h"
#include "dat_ents.h"
#include "dat_maps.h"
#include "dat_screens.h"
#include "dat_sprites.h"
#include "dat_tiles.h"
#include "dat_pics.h"
#include "system/basic_funcs.h"
#include "img.h"
#ifdef GFXPC
#include "dat_palettePC.e"
#endif
#ifdef GFXST
#include "dat_paletteST.e"
#endif

#include <stdio.h>

/*-------------------------------------------------------*/

/*
 * miniz used only for crc32 calculation
 */
#define MINIZ_NO_STDIO
#define MINIZ_NO_TIME
#define MINIZ_NO_ARCHIVE_APIS
#define MINIZ_NO_ARCHIVE_WRITING_APIS
#define MINIZ_NO_ZLIB_APIS
#define MINIZ_NO_MALLOC
#include "miniz/miniz.c"

/*-------------------------------------------------------*/

static const char * resourceFiles[Resource_MAX_COUNT] = 
{
    BOOTSTRAP_RESOURCE_NAME,
    "graphics/palette.dat",
    "misc/entdata.dat",
    "misc/sprseq.dat",
    "misc/mvstep.dat",
    "misc/maps.dat",
    "misc/submaps.dat",
    "misc/connect.dat",
    "misc/bnums.dat",
    "misc/blocks.dat",
    "misc/marks.dat",
    "misc/eflgc.dat",
    "misc/imapsl.dat",
    "misc/imapsteps.dat",
    "misc/imapsofs.dat",
    "texts/imaptext.dat",
    "texts/gameovertxt.dat",
    "texts/pausedtxt.dat",
    "graphics/spritesdata.dat",
    "graphics/tilesdata.dat",
    "misc/highscores.dat",
#ifdef GFXST
    "graphics/pichaf.dat", /* ST version only */
    "graphics/piccongrats.dat", /* ST version only */
    "graphics/picsplash.dat", /* ST version only */
#else
    NULL, 
    NULL, 
    NULL,
#endif /* GFXST */
#ifdef GFXPC
    "graphics/screenimainhoft.dat", /* PC version only */
    "graphics/screenimainrdt.dat", /* PC version only */
    "graphics/screenimaincdc.dat", /* PC version only */
    "graphics/screencongrats.dat", /* PC version only */
#else
    NULL, 
    NULL, 
    NULL, 
    NULL
#endif /* GFXPC */
};

/*-------------------------------------------------------*/

/*
 * write 16b length + not-terminated string
 */
static bool writeString(FILE * fp, const char * name, const char terminator)
{
    U16 u16Temp;

    const char * p = name;
    size_t length = 0;
    if (name)
    {
        while (*p != terminator)
        {
            ++p, ++length;
            if (length > 0xFFFF)
            {
                fprintf(stderr, "Error: string too long\n");
                return false;
            }
        }
    }

    u16Temp = htole16((U16)length);
    if (fwrite(&u16Temp, sizeof(u16Temp), 1, fp) != 1)
    {
        perror("fwrite()");
        return false;
    }

    if (name)
    {
        if (fwrite(name, length, 1, fp) != 1)
        {
            perror("fwrite()");
            return false;
        }
    }
    return true;
}

/*-------------------------------------------------------*/

static bool writeRawData(FILE * fp, const void * buffer, const size_t size, const size_t count)
{
    U16 u16Temp = htole16(count);
    if (fwrite(&u16Temp, sizeof(u16Temp), 1, fp) != 1)
    {
        perror("fwrite()");
        return false;
    }

    if (fwrite(buffer, size, count, fp) != count)
    {
        perror("fwrite()");
        return false;
    }
    return true;
}

/*-------------------------------------------------------*/

#ifdef GFXST
static bool writePicture(FILE * fp, const pic_t * picture)
{
    U16 u16Temp;
    U32 u32Temp;
    size_t i, pixelWords32b;
    resource_pic_t dataTemp;

    u16Temp = htole16(picture->width);
    memcpy(dataTemp.width, &u16Temp, sizeof(U16));
    u16Temp = htole16(picture->height);
    memcpy(dataTemp.height, &u16Temp, sizeof(U16));
    u16Temp = htole16(picture->xPos);
    memcpy(dataTemp.xPos, &u16Temp, sizeof(U16));
    u16Temp = htole16(picture->yPos);
    memcpy(dataTemp.yPos, &u16Temp, sizeof(U16));
    if (fwrite(&dataTemp, sizeof(dataTemp), 1, fp) != 1)
    {
        perror("fwrite()");
        return false;
    }  

    pixelWords32b = (picture->width * picture->height) / 8;  /*we use 4b per pixel*/
    for (i = 0; i < pixelWords32b; ++i)
    {
        u32Temp = htole32(picture->pixels[i]);
        if (fwrite(&u32Temp, sizeof(u32Temp), 1, fp) != 1)
        {
            perror("fwrite()");
            return false;
        }  
    }
    return true;
}
#endif /* GFXST */

/*-------------------------------------------------------*/

static bool writeResourceFilelist(FILE * fp)
{
    unsigned id;

    for (id = Resource_PALETTE; id < Resource_MAX_COUNT; ++id)
    {
        if (!writeString(fp, resourceFiles[id], 0x00))
        {
            return false;
        }
    }
    return true;
}

/*-------------------------------------------------------*/

static bool writeResourceEntdata(FILE * fp)
{
    size_t i;
    U16 u16Temp;
    resource_entdata_t dataTemp;

    u16Temp = htole16(ENT_NBR_ENTDATA);
    if (fwrite(&u16Temp, sizeof(u16Temp), 1, fp) != 1)
    {
        perror("fwrite()");
        return false;
    }

    for (i = 0; i < ENT_NBR_ENTDATA; ++i)
    {
        dataTemp.w = ENT_ENTDATA[i].w;
        dataTemp.h = ENT_ENTDATA[i].h;
        u16Temp = htole16(ENT_ENTDATA[i].spr);
        memcpy(dataTemp.spr, &u16Temp, sizeof(U16));
        u16Temp = htole16(ENT_ENTDATA[i].sni);
        memcpy(dataTemp.sni, &u16Temp, sizeof(U16));
        dataTemp.trig_w = ENT_ENTDATA[i].trig_w;
        dataTemp.trig_h = ENT_ENTDATA[i].trig_h;
        dataTemp.snd = ENT_ENTDATA[i].snd;
        if (fwrite(&dataTemp, sizeof(dataTemp), 1, fp) != 1)
        {
            perror("fwrite()");
            return false;
        }
    }
    return true;
}

/*-------------------------------------------------------*/

static bool writeResourceMaps(FILE * fp)
{
    size_t i;
    U16 u16Temp;
    resource_map_t dataTemp;

    u16Temp = htole16(MAP_NBR_MAPS);
    if (fwrite(&u16Temp, sizeof(u16Temp), 1, fp) != 1)
    {
        perror("fwrite()");
        return false;
    }

    for (i = 0; i < MAP_NBR_MAPS; ++i)
    {
        u16Temp = htole16(MAP_MAPS[i].x);
        memcpy(dataTemp.x, &u16Temp, sizeof(U16));
        u16Temp = htole16(MAP_MAPS[i].y);
        memcpy(dataTemp.y, &u16Temp, sizeof(U16));
        u16Temp = htole16(MAP_MAPS[i].row);
        memcpy(dataTemp.row, &u16Temp, sizeof(U16));
        u16Temp = htole16(MAP_MAPS[i].submap);
        memcpy(dataTemp.submap, &u16Temp, sizeof(U16));
        if (fwrite(&dataTemp, sizeof(dataTemp), 1, fp) != 1)
        {
            perror("fwrite()");
            return false;
        }
        if (!writeString(fp, MAP_MAPS[i].tune, 0x00))
        {
            return false;
        }
    }     
    return true;
}

/*-------------------------------------------------------*/

static bool writeResourceSubmaps(FILE * fp)
{
    size_t i;
    U16 u16Temp;
    resource_submap_t dataTemp;

    u16Temp = htole16(MAP_NBR_SUBMAPS);
    if (fwrite(&u16Temp, sizeof(u16Temp), 1, fp) != 1)
    {
        perror("fwrite()");
        return false;
    }

    for (i = 0; i < MAP_NBR_SUBMAPS; ++i) 
    {
        u16Temp = htole16(MAP_SUBMAPS[i].page);
        memcpy(dataTemp.page, &u16Temp, sizeof(U16));
        u16Temp = htole16(MAP_SUBMAPS[i].bnum);
        memcpy(dataTemp.bnum, &u16Temp, sizeof(U16));
        u16Temp = htole16(MAP_SUBMAPS[i].connect);
        memcpy(dataTemp.connect, &u16Temp, sizeof(U16));
        u16Temp = htole16(MAP_SUBMAPS[i].mark);
        memcpy(dataTemp.mark, &u16Temp, sizeof(U16));
        if (fwrite(&dataTemp, sizeof(dataTemp), 1, fp) != 1)
        {
            perror("fwrite()");
            return false;
        }  
    }  
    return true;
}
 
/*-------------------------------------------------------*/

static bool writeResourceImapsteps(FILE * fp)
{
    size_t i;
    U16 u16Temp;
    resource_imapsteps_t dataTemp;

    u16Temp = htole16(SCREEN_NBR_IMAPSTESPS);
    if (fwrite(&u16Temp, sizeof(u16Temp), 1, fp) != 1)
    {
        perror("fwrite()");
        return false;
    }

    for (i = 0; i < SCREEN_NBR_IMAPSTESPS; ++i) 
    {
        u16Temp = htole16(SCREEN_IMAPSTEPS[i].count);
        memcpy(dataTemp.count, &u16Temp, sizeof(U16));
        u16Temp = htole16(SCREEN_IMAPSTEPS[i].dx);
        memcpy(dataTemp.dx, &u16Temp, sizeof(U16));
        u16Temp = htole16(SCREEN_IMAPSTEPS[i].dy);
        memcpy(dataTemp.dy, &u16Temp, sizeof(U16));
        u16Temp = htole16(SCREEN_IMAPSTEPS[i].base);
        memcpy(dataTemp.base, &u16Temp, sizeof(U16));
        if (fwrite(&dataTemp, sizeof(dataTemp), 1, fp) != 1)
        {
            perror("fwrite()");
            return false;
        }  
    }  
    return true;   
}

/*-------------------------------------------------------*/

static bool writeResourceImaptext(FILE * fp)
{
    size_t i;

    U16 u16Temp = htole16(SCREEN_NBR_IMAPTEXT);
    if (fwrite(&u16Temp, sizeof(u16Temp), 1, fp) != 1)
    {
        perror("fwrite()");
        return false;
    }

    for (i = 0; i < SCREEN_NBR_IMAPTEXT; ++i) 
    {
        if (!writeString(fp, SCREEN_IMAPTEXT[i], 0xFE))
        {
            perror("fwrite()");
            return false;
        }  
    }
    return true;
}

/*-------------------------------------------------------*/

static bool writeResourceSpritesData(FILE * fp)
{
    size_t i, j;

    U16 u16Temp = htole16(SPRITES_NBR_SPRITES);
    if (fwrite(&u16Temp, sizeof(u16Temp), 1, fp) != 1)
    {
        perror("fwrite()");
        return false;
    }

#ifdef GFXST
    for (i = 0; i < SPRITES_NBR_SPRITES; ++i)
    {
        for (j = 0; j < SPRITES_NBR_DATA; ++j) 
        {
            U32 u32Temp = htole32(SPRITES_DATA[i][j]);
            if (fwrite(&u32Temp, sizeof(u32Temp), 1, fp) != 1)
            {
                perror("fwrite()");
                return false;
            }
        }
    }
#endif /* GFXST */

#ifdef GFXPC
    for (i = 0; i < SPRITES_NBR_SPRITES; ++i)
    {
        for (j = 0; j < SPRITES_NBR_COLS; ++j) 
        {
            size_t k;
            for (k = 0; k < SPRITES_NBR_ROWS; ++k) 
            {
                resource_spriteX_t dataTemp;

                u16Temp = htole16(SPRITES_DATA[i][j][k].mask);
                memcpy(dataTemp.mask, &u16Temp, sizeof(U16));
                u16Temp = htole16(SPRITES_DATA[i][j][k].pict);
                memcpy(dataTemp.pict, &u16Temp, sizeof(U16));
                if (fwrite(&dataTemp, sizeof(dataTemp), 1, fp) != 1)
                {
                    perror("fwrite()");
                    return false;
                }
            }
        }              
    }
#endif /* GFXPC */

    return true;
}

/*-------------------------------------------------------*/

static bool writeResourceTilesData(FILE * fp)
{
    size_t i, j, k;

    U16 u16Temp = htole16(TILES_NBR_BANKS);
    if (fwrite(&u16Temp, sizeof(u16Temp), 1, fp) != 1)
    {
        perror("fwrite()");
        return false;
    }

    for (i = 0; i < TILES_NBR_BANKS ; ++i)
    {
        for (j = 0; j < TILES_NBR_TILES; ++j) 
        {
            for (k = 0; k < TILES_NBR_LINES ; ++k)
            {
#ifdef GFXPC
                U16 dataTemp = htole16(TILES_DATA[i][j][k]);
#endif
#ifdef GFXST
                U32 dataTemp = htole32(TILES_DATA[i][j][k]);
#endif
                if (fwrite(&dataTemp, sizeof(dataTemp), 1, fp) != 1)
                {
                    perror("fwrite()");
                    return false;
                }
            }
        }              
    }
    return true;
}

/*-------------------------------------------------------*/

static bool writeResourceHighScores(FILE * fp)
{
    size_t i;
    U16 u16Temp;
    U32 u32Temp;
    resource_hiscore_t dataTemp;

    u16Temp = htole16(SCREEN_NBR_HISCORES);
    if (fwrite(&u16Temp, sizeof(u16Temp), 1, fp) != 1)
    {
        perror("fwrite()");
        return false;
    }

    for (i = 0; i < SCREEN_NBR_HISCORES; ++i) 
    {
        u32Temp = htole32(SCREEN_HIGHSCORES[i].score);
        memcpy(dataTemp.score, &u32Temp, sizeof(U32));
        memcpy(dataTemp.name, SCREEN_HIGHSCORES[i].name, HISCORE_NAME_SIZE);
        if (fwrite(&dataTemp, sizeof(dataTemp), 1, fp) != 1)
        {
            perror("fwrite()");
            return false;
        }  
    }  
    return true;
}


/*-------------------------------------------------------*/

static bool writeHeader(FILE * fp, const unsigned id)
{
    resource_header_t header;
    U16 u16Temp;

    memcpy(header.magic, resource_magic, sizeof(header.magic));
    u16Temp = htole16(DATA_VERSION);
    memcpy(header.version, &u16Temp, sizeof(header.version));
    u16Temp = htole16((U16)id);
    memcpy(header.resourceId, &u16Temp, sizeof(header.resourceId));
    if (fwrite(&header, sizeof(header), 1, fp) != 1)
    {
        perror("fwrite()");
        return false;
    }
    return true;
}

/*-------------------------------------------------------*/

static bool writeCrc32(FILE * fp)
{
    U8 tempBuffer[1024];
    U32 crc32 = MZ_CRC32_INIT;

    if (fflush(fp) != 0)
    {       
        perror("fflush()");
        return false;
    }
    rewind(fp);

    for (;;)
    {
        size_t bytesRead = fread(tempBuffer, sizeof(U8), sizeof(tempBuffer), fp);
        bool isEndOfFile = (feof(fp) != 0);
        if (bytesRead != sizeof(tempBuffer) && !isEndOfFile)
        {
            perror("fread()");
            return false;
        }

        crc32 = mz_crc32(crc32, tempBuffer, bytesRead);

        if (isEndOfFile)
        {
            break;
        }
    }

    crc32 = htole32(crc32);
    if (fwrite(&crc32, sizeof(crc32), 1, fp) != 1)
    {
        perror("fwrite()");
        return false;
    }
    return true;
}

/*-------------------------------------------------------*/

static bool writeFile(const unsigned id)
{
    bool success;
    FILE * fp;

    fp = fopen(resourceFiles[id], "wb+");
    if (fp == NULL)
    {
        perror("open()");
        return false;
    }

    success = writeHeader(fp, id);

    if (success) 
    {
        switch (id)
        {
            case Resource_FILELIST: success = writeResourceFilelist(fp); break;
            case Resource_PALETTE: success = writeRawData(fp, GAME_COLORS, sizeof(*GAME_COLORS), GAME_COLOR_COUNT); break;
            case Resource_ENTDATA: success = writeResourceEntdata(fp); break;
            case Resource_SPRSEQ: success = writeRawData(fp, ENT_SPRSEQ, sizeof(*ENT_SPRSEQ), ENT_NBR_SPRSEQ); break;
            case Resource_MVSTEP: success = writeRawData(fp, ENT_MVSTEP, sizeof(*ENT_MVSTEP), ENT_NBR_MVSTEP); break;
            case Resource_MAPS: success = writeResourceMaps(fp); break;
            case Resource_SUBMAPS: success = writeResourceSubmaps(fp); break;
            case Resource_CONNECT: success = writeRawData(fp, MAP_CONNECT, sizeof(*MAP_CONNECT), MAP_NBR_CONNECT); break;
            case Resource_BNUMS: success = writeRawData(fp, MAP_BNUMS, sizeof(*MAP_BNUMS), MAP_NBR_BNUMS); break;
            case Resource_BLOCKS: success = writeRawData(fp, MAP_BLOCKS, sizeof(*MAP_BLOCKS), MAP_NBR_BLOCKS); break;
            case Resource_MARKS: success = writeRawData(fp, MAP_MARKS, sizeof(*MAP_MARKS), MAP_NBR_MARKS); break;
            case Resource_EFLGC: success = writeRawData(fp, MAP_EFLG_C, sizeof(*MAP_EFLG_C), MAP_NBR_EFLGC); break;
            case Resource_IMAPSL: success = writeRawData(fp, SCREEN_IMAPSL, sizeof(*SCREEN_IMAPSL), SCREEN_NBR_IMAPSL); break;
            case Resource_IMAPSTEPS: success = writeResourceImapsteps(fp); break;
            case Resource_IMAPSOFS: success = writeRawData(fp, SCREEN_IMAPSOFS, sizeof(*SCREEN_IMAPSOFS), SCREEN_NBR_IMAPSOFS); break;
            case Resource_IMAPTEXT: success = writeResourceImaptext(fp); break;
            case Resource_GAMEOVERTXT: success = writeString(fp, SCREEN_GAMEOVERTXT, 0xFE); break;
            case Resource_PAUSEDTXT: success = writeString(fp, SCREEN_PAUSEDTXT, 0xFE); break;           
            case Resource_SPRITESDATA: success = writeResourceSpritesData(fp); break;
            case Resource_TILESDATA: success = writeResourceTilesData(fp); break;
            case Resource_HIGHSCORES: success = writeResourceHighScores(fp); break;
#ifdef GFXST
            case Resource_PICHAF: success = writePicture(fp, PIC_HAF); break;
            case Resource_PICCONGRATS: success = writePicture(fp, PIC_CONGRATS); break;
            case Resource_PICSPLASH: success = writePicture(fp, PIC_SPLASH); break;
#endif /* GFXST */
#ifdef GFXPC
            case Resource_IMAINHOFT: success = writeString(fp, SCREEN_IMAINHOFT, 0xFE); break;
            case Resource_IMAINRDT: success = writeString(fp, SCREEN_IMAINRDT, 0xFE); break;
            case Resource_IMAINCDC: success = writeString(fp, SCREEN_IMAINCDC, 0xFE); break;
            case Resource_SCREENCONGRATS: success = writeString(fp, SCREEN_CONGRATS, 0xFE); break;
#endif /* GFXPC */
            default: success = false; break;
        }
    }

    if (success) 
    {
        success = writeCrc32(fp);
    }

    fflush(fp);
    fclose(fp);
    return success;
}

/*-------------------------------------------------------*/

int main(int argc, char *argv[])
{
    bool success = true;
    unsigned id;

    for (id = Resource_FILELIST; (id < Resource_MAX_COUNT) && success; ++id)
    {
        if (resourceFiles[id])
        {
            success = writeFile(id);
        }
    }
    return !success;
}
