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

#include "xrick/config.h"
#include "xrick/resources.h"
#include "data_extractor/dat_ents.h"
#include "data_extractor/dat_maps.h"
#include "data_extractor/dat_screens.h"
#include "data_extractor/dat_sprites.h"
#include "data_extractor/dat_tiles.h"
#include "data_extractor/dat_pics.h"
#include "xrick/system/basic_funcs.h"
#include "xrick/img.h"
#include "data_extractor/dat_imgs.e"
#ifdef GFXPC
#include "data_extractor/dat_palettePC.e"
#endif
#ifdef GFXST
#include "data_extractor/dat_paletteST.e"
#endif

#include "xrick/system/miniz_config.h"
#include "miniz/miniz.c"

#include <stdio.h>

#ifdef _MSC_VER
#  define snprintf _snprintf
#endif

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
    "graphics/imgsplash.dat",
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
    NULL,
#endif /* GFXPC */
    "sounds/bombshht.wav",
    "sounds/bonus.wav",
    "sounds/box.wav",
    "sounds/bullet.wav",
    "sounds/crawl.wav",
    "sounds/die.wav",
    "sounds/ent0.wav",
    "sounds/ent1.wav",
    "sounds/ent2.wav",
    "sounds/ent3.wav",
    "sounds/ent4.wav",
    "sounds/ent5.wav",
    "sounds/ent6.wav",
    "sounds/ent7.wav",
    "sounds/ent8.wav",
    "sounds/explode.wav",
    "sounds/gameover.wav",
    "sounds/jump.wav",
    "sounds/pad.wav",
    "sounds/sbonus1.wav",
    "sounds/sbonus2.wav",
    "sounds/stick.wav",
    "sounds/tune0.wav",
    "sounds/tune1.wav",
    "sounds/tune2.wav",
    "sounds/tune3.wav",
    "sounds/tune4.wav",
    "sounds/tune5.wav",
    "sounds/walk.wav"
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

static bool writeImage(FILE * fp, const img_t * image)
{
    U16 u16Temp;
    size_t pixelCount;
    resource_pic_t dataTemp;

    u16Temp = htole16(image->width);
    memcpy(dataTemp.width, &u16Temp, sizeof(U16));
    u16Temp = htole16(image->height);
    memcpy(dataTemp.height, &u16Temp, sizeof(U16));
    u16Temp = htole16(image->xPos);
    memcpy(dataTemp.xPos, &u16Temp, sizeof(U16));
    u16Temp = htole16(image->yPos);
    memcpy(dataTemp.yPos, &u16Temp, sizeof(U16));
    if (fwrite(&dataTemp, sizeof(dataTemp), 1, fp) != 1)
    {
        perror("fwrite()");
        return false;
    }

    if (!writeRawData(fp, image->colors, sizeof(*image->colors), image->ncolors))
    {
        return false;
    }

    pixelCount = (image->width * image->height);  /*we use 8b per pixel*/
    if (fwrite(image->pixels, sizeof(*image->pixels), pixelCount, fp) != pixelCount)
    {
        perror("fwrite()");
        return false;
    }
    return true;
}

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
        u16Temp = htole16(Resource_SOUNDTUNE0 + i);
        memcpy(dataTemp.tuneId, &u16Temp, sizeof(U16));
        if (fwrite(&dataTemp, sizeof(dataTemp), 1, fp) != 1)
        {
            perror("fwrite()");
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

static bool writeFile(const unsigned id, char * rootPath)
{
    bool success;
    FILE * fp;
    char fullPath[512];

    snprintf(fullPath, sizeof(fullPath), "%s/%s", rootPath, resourceFiles[id]);
    printf("\"%s\"...", fullPath);
    fp = fopen(fullPath, "wb+");
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
            case Resource_IMGSPLASH: success = writeImage(fp, IMG_SPLASH); break;
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

static void printHelp(void)
{
   printf(
       "Usage: data_extractor [option(s)]\n"
       " The options are:\n\n"
       "  -h, --help         Display this information\n"
       "  --output <path>    Use output path <path>. <path> must be a directory.\n"
       "                     The default is to generate data files in \".\".\n\n"
       );
}

/*-------------------------------------------------------*/

static void printFailure(char *msg)
{
    printf(
        "data_extractor: %s\n"
        " Use 'data_extractor --help' for a complete list of options.\n", msg);
}

/*-------------------------------------------------------*/

int main(int argc, char *argv[])
{
    bool success = true;
    unsigned id;
    int argIdx;
    char * rootPath = ".";

    for (argIdx = 1; argIdx < argc; argIdx++) 
    {
        if (!strcmp(argv[argIdx], "--help") ||
            !strcmp(argv[argIdx], "-h")) 
        {
            printHelp();
            success = false;
        }
        else if (!strcmp(argv[argIdx], "--output")) 
        {
            if (++argIdx == argc) 
            {
                printFailure("missing output path");
                success = false;
            }
            rootPath = argv[argIdx];
        }
        else 
        {
            char message[128];
            snprintf(message, sizeof(message), "unrecognized option '%s'", argv[argIdx]);
            printFailure(message);
            success = false;
        }
    }

    /* note: at the moment we store sounds as plain WAVE files (e.g. no custom header/crc) */ 
    for (id = Resource_FILELIST; (id <= Resource_SCREENCONGRATS) && success; ++id)
    {
        if (resourceFiles[id])
        {
            printf("Generating file ");
			
            success = writeFile(id, rootPath);
            
			printf(success? "done.\n" : "error!\n");
        }
    }
    return (success? 0 : 1);
}
