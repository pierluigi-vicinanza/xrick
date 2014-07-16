/*
 * xrick/system/sysmem.c
 *
 * Copyright (C) 2008-2013 Pierluigi Vicinanza. All rights reserved.
 *
 * The use and distribution terms for this software are contained in the file
 * named README, which can be found in the root of this distribution. By
 * using this software in any fashion, you are agreeing to be bound by the
 * terms of this license.
 *
 * You must not remove this notice, or any other, from this software.
 */

#include "xrick/system/system.h"
#include "xrick/debug.h"

/*
 * local vars
 */
enum 
{ 
    STACK_MAX_SIZE = 192*1024,
    ALIGNMENT = sizeof(void*)  /* this is more of an educated guess; might want to adjust for your specific architecture */
};
static U8 stackBuffer[STACK_MAX_SIZE];
static U8 * stackTop;
static size_t stackSize;
IFDEBUG_MEMORY( static size_t maxUsedMemory = 0; );

/*
 * Initialise memory stack
 */
bool sysmem_init(void)
{
    stackTop = stackBuffer;
    stackSize = 0;
    return true;
}

/*
 * Cleanup memory stack
 */
void sysmem_shutdown(void)
{
    if (stackTop != stackBuffer || stackSize != 0)
    {
        sys_printf("(memory) improper deallocation detected");
    }

    IFDEBUG_MEMORY(
        sys_printf("xrick/memory: max memory usage was %u bytes\n", maxUsedMemory);
    );
}

/*
 * Allocate a memory-aligned block on top of the memory stack
 */
void *sysmem_push(size_t size)
{
    uintptr_t alignedPtr;
    size_t * allocatedSizePtr;

    size_t neededSize = sizeof(size_t) + size + (ALIGNMENT - 1);
    if (stackSize + neededSize > STACK_MAX_SIZE)
    {
        sys_error("(memory) tried to allocate a block when memory full");
        return NULL;
    }

    alignedPtr = (((uintptr_t)stackTop) + sizeof(size_t) + ALIGNMENT) & ~((uintptr_t)(ALIGNMENT - 1));
    
    allocatedSizePtr = (size_t *)(alignedPtr);
    allocatedSizePtr[-1] = neededSize;

    stackTop += neededSize;
    stackSize += neededSize;

    IFDEBUG_MEMORY(
        sys_printf("xrick/memory: allocated %u bytes\n", neededSize);
        if (stackSize > maxUsedMemory) maxUsedMemory = stackSize;
    );

    return (void *)alignedPtr;
}

/*
 * Release block from the top of the memory stack
 */
void sysmem_pop(void * alignedPtr)
{
    size_t allocatedSize;

    if (!alignedPtr) 
    {
        return;
    }

    if (stackSize == 0)
    {
        sys_error("(memory) tried to release a block when memory empty");
        return;
    }

    allocatedSize = ((size_t *)(alignedPtr))[-1];
    stackTop -= allocatedSize;
    stackSize -= allocatedSize;

    IFDEBUG_MEMORY(
        if ((uintptr_t)alignedPtr != ((((uintptr_t)stackTop) + sizeof(size_t) + ALIGNMENT) & ~((uintptr_t)(ALIGNMENT - 1))))
        {
            sys_error("(memory) tried to release a wrong block");
            return;
        }
    );

    IFDEBUG_MEMORY(
        sys_printf("xrick/memory: released %u bytes\n", allocatedSize);
    );
}

/* eof */



