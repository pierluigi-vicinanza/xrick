/*
 * xrick/src/data.c
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

#include "xrick/system/system.h"

#ifdef ENABLE_ZIP
#include "zlib/unzip.h"
#endif
#include "xrick/util.h"

#include <stdio.h>  /* sprintf */
#include <string.h> /* strlen */
#include <sys/stat.h> /* fstat */

/* handle Microsoft Visual C */
#ifdef _MSC_VER
#define fileno _fileno
#endif

/*
 * Global variables
 */
#ifdef ENABLE_ZIP
const char *sysfile_defaultPath = "data.zip";
#else
const char *sysfile_defaultPath = ".";
#endif

/*
 * Private typedefs
 */
typedef struct 
{
	char *name;
#ifdef ENABLE_ZIP
	unzFile zip;
#endif
} path_t;

/*
 * Static variables
 */
static path_t rootPath = 
{
    NULL, 
#ifdef ENABLE_ZIP
    NULL
#endif
}; /* will store either main data folder path or main zipped archive path */

/*
 * Prototypes
 */
#ifdef ENABLE_ZIP
static int str_hasZipExtension(const char *);
#endif
static char *str_toNativeSeparators(char *);

/*
 *
 */
bool
sysfile_setRootPath(const char *name)
{
    char *path = u_strdup(name);
    if (!path)
    {
        return false;
    }

    rootPath.name = str_toNativeSeparators(path);
#ifdef ENABLE_ZIP
	if (str_hasZipExtension(rootPath.name)) 
    {
        rootPath.zip = unzOpen(rootPath.name);
        if (!rootPath.zip) 
        {
            sys_error("(sysfile) can not open zip file");
            return false;
        } 
	} 
    else /* dealing with a directory */
    {
		/* FIXME check that it is a valid directory */
		rootPath.zip = NULL;
	}
#endif  /* ENABLE_ZIP */
    return true;
}

/*
 *
 */
void
sysfile_clearRootPath()
{
#ifdef ENABLE_ZIP
	if (rootPath.zip) 
    {
        unzClose(rootPath.zip);
		rootPath.zip = NULL;
	}
#endif  /* ENABLE_ZIP */
	sysmem_pop(rootPath.name);
	rootPath.name = NULL;
}

/*
 * Open a data file.
 */
file_t
sysfile_open(const char *name)
{
#ifdef ENABLE_ZIP
    if (rootPath.zip) 
    {
        unzFile zh = rootPath.zip;
        if (unzLocateFile(zh, name, 0) != UNZ_OK ||
            unzOpenCurrentFile(zh) != UNZ_OK) 
        {
                return NULL;
        }
        return (file_t)zh;
    }
    else /* uncompressed file */
#endif /* ENABLE_ZIP */
    {
        FILE *fh;
        char *fullPath = sysmem_push(strlen(rootPath.name) + strlen(name) + 2);
        if (!fullPath)
        {
            return NULL;
        }
        sprintf(fullPath, "%s/%s", rootPath.name, name);
        str_toNativeSeparators(fullPath);
        fh = fopen(fullPath, "rb");
        sysmem_pop(fullPath);
        return (file_t)fh;
    }
}

/*
 * 
 */
off_t
sysfile_size(file_t file)
{
	off_t size = -1;
#ifdef ENABLE_ZIP
    if (rootPath.zip)
    {
        /* not implemented */
    } 
    else
#endif /* ENABLE_ZIP */
    {
        FILE *fh = (FILE *)file;
        int fd = fileno(fh);
        struct stat fileStat;
        if (fstat(fd, &fileStat) == 0)
        {
            size = fileStat.st_size;
        }
    }
	return size;
}

/*
 * Seek.
 */
int
sysfile_seek(file_t file, long offset, int origin)
{
#ifdef ENABLE_ZIP
	if (rootPath.zip) 
    {
		/* not implemented */
		return -1;
	} 
    else
#endif /* ENABLE_ZIP */
    {
		return fseek((FILE *)file, offset, origin);
	}
}

/*
 * Tell.
 */
int
sysfile_tell(file_t file)
{
#ifdef ENABLE_ZIP
	if (rootPath.zip) 
    {
		/* not implemented */
		return -1;
	} 
    else
#endif /* ENABLE_ZIP */
    {
		return ftell((FILE *)file);
	}
}

/*
 * Read a file within a data archive.
 */
int
sysfile_read(file_t file, void *buf, size_t size, size_t count)
{
#ifdef ENABLE_ZIP
	if (rootPath.zip) 
    {
		return unzReadCurrentFile((unzFile)file, buf, size * count) / size;
	} 
    else
#endif /* ENABLE_ZIP */
    {
		return fread(buf, size, count, (FILE *)file);
	}
}

/*
 * Close a file within a data archive.
 */
void
sysfile_close(file_t file)
{
#ifdef ENABLE_ZIP
	if (rootPath.zip) 
    {
        unzCloseCurrentFile((unzFile)file);
	}
    else
#endif /* ENABLE_ZIP */
    {
		fclose((FILE *)file);
	}
}

#ifdef ENABLE_ZIP
/*
 * Returns 1 if filename has .zip extension.
 */
static int
str_hasZipExtension(const char *name)
{
	int i;

	i = strlen(name) - 1;
	if (i < 0 || name[i] != 'p' && name[i] != 'P') return 0;
	i--;
	if (i < 0 || name[i] != 'i' && name[i] != 'I') return 0;
	i--;
	if (i < 0 || name[i] != 'z' && name[i] != 'Z') return 0;
	i--;
	if (i < 0 || name[i] != '.') return 0;
	i--;
	if (i < 0) return 0;
	return 1;
}
#endif /* ENABLE_ZIP */

/*
 *
 */
static char *
str_toNativeSeparators(char *s)
{
#ifdef __WIN32__
	int i, l;

	l = strlen(s);
	for (i = 0; i < l; i++)
		if (s[i] == '/') s[i] = '\\';
#endif
	return s;
}

/* eof */
