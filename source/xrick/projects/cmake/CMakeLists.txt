#
# xrick/projects/cmake/CMakeLists.txt
# 
# Copyright (C) 2008-2014 Pierluigi Vicinanza. All rights reserved.
#
# The use and distribution terms for this software are contained in the file
# named README, which can be found in the root of this distribution. By
# using this software in any fashion, you are agreeing to be bound by the
# terms of this license.
#
# You must not remove this notice, or any other, from this software.
#

cmake_minimum_required(VERSION 2.8.5)

project(xrick C)

set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../..)

set(XRICK_VERSION_MAJOR 2)
set(XRICK_VERSION_MINOR 0)
set(XRICK_VERSION_PATCH 0)
set(XRICK_VERSION_STR "${XRICK_VERSION_MAJOR}.${XRICK_VERSION_MINOR}.${XRICK_VERSION_PATCH}")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif(NOT CMAKE_BUILD_TYPE)
message(STATUS ${PROJECT_NAME} " build type: " ${CMAKE_BUILD_TYPE})

#-----------------------------------------------------------------------------
# Find and set ZLIB
#
set(ZLIB_PREFIX "" CACHE PATH 
    "The location of the ZLIB install prefix (only used if the ZLIB is not yet found)")
if(ZLIB_PREFIX)
    set(CMAKE_PREFIX_PATH ${ZLIB_PREFIX})
endif()

find_package(ZLIB)

if(NOT ZLIB_FOUND)
    message(FATAL_ERROR
           "Could not find a ZLIB installation.\n"
           "Set ZLIB_PREFIX to the location where ZLIB is installed.\n")
else()
    message(STATUS "Cmake FindZLIB: using ZLIB includes at: ${ZLIB_INCLUDE_DIR}")
    message(STATUS "Cmake FindZLIB: using ZLIB libraries: ${ZLIB_LIBRARY}")
endif()

include_directories(${ZLIB_INCLUDE_DIR})
list(APPEND LIBS ${ZLIB_LIBRARY})

#-----------------------------------------------------------------------------
# Find and set SDL
#
set(SDL_PREFIX "" CACHE PATH 
    "The location of the SDL install prefix (only used if the SDL is not yet found)")
if(SDL_PREFIX)
    set(CMAKE_PREFIX_PATH ${SDL_PREFIX})
endif()

find_package(SDL)

if(NOT SDL_FOUND)
    message(FATAL_ERROR
            "Could not find a SDL installation.\n"
            "Set SDL_PREFIX to the location where SDL is installed.\n")
elseif(NOT SDL_VERSION_STRING MATCHES "(1\\.2\\.[0-9]+)")
    message(FATAL_ERROR
            "Unsupported SDL version.\n"
            "Detected SDL version ${SDL_VERSION_STRING}, required 1.2.x.\n")
else()
    message(STATUS "Cmake FindSDL: using SDL includes at: ${SDL_INCLUDE_DIR}")
    message(STATUS "Cmake FindSDL: using SDL libraries: ${SDL_LIBRARY}")
endif()

include_directories(${SDL_INCLUDE_DIR})
list(APPEND LIBS ${SDL_LIBRARY})

#-----------------------------------------------------------------------------
# Options
#
option(GFXST "Use Atari ST graphics" ON)
option(GFXPC "Use PC graphics" OFF)
if ((NOT GFXST AND NOT GFXPC) OR (GFXST AND GFXPC))
    set(GFXST true CACHE BOOL "Use Atari ST graphics" FORCE)
    set(GFXPC false CACHE BOOL "Use PC graphics" FORCE)
    message(WARNING
            "Must choose support for one graphic format only.\n"
            "Defaulting to Atari ST graphics.")
endif()
option(ENABLE_JOYSTICK "Enable joystick support" OFF)
option(ENABLE_SOUND "Enable sound" ON)
option(ENABLE_CHEATS "Enable cheats" ON)
option(ENABLE_FOCUS "Enable auto-defocus support" OFF)
option(ENABLE_DEVTOOLS "Enable development tools" OFF)
option(DEBUG_MEMORY "Enable memory debugging support" OFF)
option(DEBUG_ENTS "Enable entity debugging support" OFF)
option(DEBUG_SCROLLER "Enable scroller debugging support" OFF)
option(DEBUG_MAPS "Enable map debugging support" OFF)
option(DEBUG_JOYSTICK  "Enable joystick debugging support" OFF)
option(DEBUG_EVENTS "Enable event debugging support" OFF)
option(DEBUG_AUDIO "Enable audio debugging support" OFF)
option(DEBUG_AUDIO2 "Enable extra audio debugging support" OFF)
option(DEBUG_VIDEO "Enable video debugging support" OFF)
option(DEBUG_VIDEO2 "Enable extra video debugging support" OFF)
option(ENABLE_ZIP "Enable compressed archive support" ON)

configure_file(${PROJECT_ROOT_DIR}/source/xrick/projects/cmake/config.h.in 
               ${PROJECT_ROOT_DIR}/source/xrick/config.h)

#-----------------------------------------------------------------------------
# Source files
#
set(SOURCES
    ${PROJECT_ROOT_DIR}/source/xrick/config.h
    ${PROJECT_ROOT_DIR}/source/xrick/control.c
    ${PROJECT_ROOT_DIR}/source/xrick/control.h
    ${PROJECT_ROOT_DIR}/source/xrick/debug.h
    ${PROJECT_ROOT_DIR}/source/xrick/devtools.c
    ${PROJECT_ROOT_DIR}/source/xrick/devtools.h
    ${PROJECT_ROOT_DIR}/source/xrick/draw.c
    ${PROJECT_ROOT_DIR}/source/xrick/draw.h
    ${PROJECT_ROOT_DIR}/source/xrick/e_bomb.c
    ${PROJECT_ROOT_DIR}/source/xrick/e_bomb.h
    ${PROJECT_ROOT_DIR}/source/xrick/e_bonus.c
    ${PROJECT_ROOT_DIR}/source/xrick/e_bonus.h
    ${PROJECT_ROOT_DIR}/source/xrick/e_box.c
    ${PROJECT_ROOT_DIR}/source/xrick/e_box.h
    ${PROJECT_ROOT_DIR}/source/xrick/e_bullet.c
    ${PROJECT_ROOT_DIR}/source/xrick/e_bullet.h
    ${PROJECT_ROOT_DIR}/source/xrick/e_rick.c
    ${PROJECT_ROOT_DIR}/source/xrick/e_rick.h
    ${PROJECT_ROOT_DIR}/source/xrick/e_sbonus.c
    ${PROJECT_ROOT_DIR}/source/xrick/e_sbonus.h
    ${PROJECT_ROOT_DIR}/source/xrick/e_them.c
    ${PROJECT_ROOT_DIR}/source/xrick/e_them.h
    ${PROJECT_ROOT_DIR}/source/xrick/ents.c
    ${PROJECT_ROOT_DIR}/source/xrick/ents.h
    ${PROJECT_ROOT_DIR}/source/xrick/game.c
    ${PROJECT_ROOT_DIR}/source/xrick/game.h
    ${PROJECT_ROOT_DIR}/source/xrick/maps.c
    ${PROJECT_ROOT_DIR}/source/xrick/maps.h
    ${PROJECT_ROOT_DIR}/source/xrick/rects.c
    ${PROJECT_ROOT_DIR}/source/xrick/rects.h
    ${PROJECT_ROOT_DIR}/source/xrick/res_magic.c
    ${PROJECT_ROOT_DIR}/source/xrick/resources.c
    ${PROJECT_ROOT_DIR}/source/xrick/resources.h
    ${PROJECT_ROOT_DIR}/source/xrick/scr_gameover.c
    ${PROJECT_ROOT_DIR}/source/xrick/scr_getname.c
    ${PROJECT_ROOT_DIR}/source/xrick/scr_imain.c
    ${PROJECT_ROOT_DIR}/source/xrick/scr_imap.c
    ${PROJECT_ROOT_DIR}/source/xrick/scr_pause.c
    ${PROJECT_ROOT_DIR}/source/xrick/scr_xrick.c
    ${PROJECT_ROOT_DIR}/source/xrick/screens.h
    ${PROJECT_ROOT_DIR}/source/xrick/scroller.c
    ${PROJECT_ROOT_DIR}/source/xrick/scroller.h
    ${PROJECT_ROOT_DIR}/source/xrick/util.c
    ${PROJECT_ROOT_DIR}/source/xrick/util.h
    ${PROJECT_ROOT_DIR}/source/xrick/3rd_party/zlib/ioapi.c
    ${PROJECT_ROOT_DIR}/source/xrick/3rd_party/zlib/ioapi.h
    ${PROJECT_ROOT_DIR}/source/xrick/3rd_party/zlib/unzip.c
    ${PROJECT_ROOT_DIR}/source/xrick/3rd_party/zlib/unzip.h
    ${PROJECT_ROOT_DIR}/source/xrick/data/img.c
    ${PROJECT_ROOT_DIR}/source/xrick/data/img.h
    ${PROJECT_ROOT_DIR}/source/xrick/data/pics.c
    ${PROJECT_ROOT_DIR}/source/xrick/data/pics.h
    ${PROJECT_ROOT_DIR}/source/xrick/data/sounds.c
    ${PROJECT_ROOT_DIR}/source/xrick/data/sounds.h
    ${PROJECT_ROOT_DIR}/source/xrick/data/sprites.c
    ${PROJECT_ROOT_DIR}/source/xrick/data/sprites.h
    ${PROJECT_ROOT_DIR}/source/xrick/data/tiles.c
    ${PROJECT_ROOT_DIR}/source/xrick/data/tiles.h
    ${PROJECT_ROOT_DIR}/source/xrick/system/basic_funcs.c
    ${PROJECT_ROOT_DIR}/source/xrick/system/basic_funcs.h
    ${PROJECT_ROOT_DIR}/source/xrick/system/basic_types.h
    ${PROJECT_ROOT_DIR}/source/xrick/system/main_sdl.c
    ${PROJECT_ROOT_DIR}/source/xrick/system/miniz_config.h
    ${PROJECT_ROOT_DIR}/source/xrick/system/sysarg_sdl.c
    ${PROJECT_ROOT_DIR}/source/xrick/system/sysevt_sdl.c
    ${PROJECT_ROOT_DIR}/source/xrick/system/sysfile_sdl.c
    ${PROJECT_ROOT_DIR}/source/xrick/system/sysjoy_sdl.c
    ${PROJECT_ROOT_DIR}/source/xrick/system/syskbd_sdl.c
    ${PROJECT_ROOT_DIR}/source/xrick/system/sysmem_sdl.c
    ${PROJECT_ROOT_DIR}/source/xrick/system/syssnd_sdl.c
    ${PROJECT_ROOT_DIR}/source/xrick/system/syssnd_sdl.h
    ${PROJECT_ROOT_DIR}/source/xrick/system/system_sdl.c
    ${PROJECT_ROOT_DIR}/source/xrick/system/system.h
    ${PROJECT_ROOT_DIR}/source/xrick/system/sysvid_sdl.c
)

if(WIN32)
    list(APPEND SOURCES ${PROJECT_ROOT_DIR}/source/xrick/projects/msvc/xrick.rc)
endif()

#-----------------------------------------------------------------------------
# Create target
#
add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE
                           ${PROJECT_ROOT_DIR}/source
                           ${PROJECT_ROOT_DIR}/source/xrick/3rd_party)
target_link_libraries(${PROJECT_NAME} ${LIBS})

if(CMAKE_COMPILER_IS_GNUCC)
    set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-std=gnu99")
endif()

if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS "_CRT_SECURE_NO_WARNINGS")
endif()

#-----------------------------------------------------------------------------
# Copy generated binary to game folder
#
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_ROOT_DIR}/game)

#-----------------------------------------------------------------------------
# Copy required DLLs to game folder (Windows only)
#
if(WIN32)

    # ZLIB
    if(EXISTS ${ZLIB_PREFIX}/zlib1.dll)
        # this is the official build of zlib as a DLL
        set(ZLIB_WIN32_DLL ${ZLIB_PREFIX}/zlib1.dll)
    else()
        get_filename_component(ZLIB_EXTRACTED_NAME ${ZLIB_LIBRARY} NAME_WE)
        get_filename_component(ZLIB_WIN32_DLL ${ZLIB_LIBRARY} PATH)
        set(ZLIB_WIN32_DLL ${ZLIB_WIN32_DLL}/${ZLIB_EXTRACTED_NAME}.dll)
    endif()
    if (EXISTS ${ZLIB_WIN32_DLL})
        message(STATUS "Found ZLIB dll, it will be copied from: ${ZLIB_WIN32_DLL}")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                          COMMAND ${CMAKE_COMMAND} -E copy_if_different
                          ${ZLIB_WIN32_DLL} ${PROJECT_ROOT_DIR}/game)
        list(APPEND CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS ${ZLIB_WIN32_DLL})                    
    endif()
    
    # SDL
    # extract SDL specific path only (e.g discard SDLmain)
    foreach(SDL_EXTRACTED_PATH ${SDL_LIBRARY})
        get_filename_component(SDL_EXTRACTED_NAME ${SDL_EXTRACTED_PATH} NAME)
        if(${SDL_EXTRACTED_NAME} MATCHES "SDL.lib")
            get_filename_component(SDL_WIN32_DLL ${SDL_EXTRACTED_PATH} PATH)
            set(SDL_WIN32_DLL ${SDL_WIN32_DLL}/SDL.dll)
        endif()
    endforeach()
    if (EXISTS ${SDL_WIN32_DLL})
         message(STATUS "Found SDL dll, it will be copied from: ${SDL_WIN32_DLL}")
         add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                            ${SDL_WIN32_DLL} ${PROJECT_ROOT_DIR}/game)
         list(APPEND CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS ${SDL_WIN32_DLL})         
    endif()

endif()

#-----------------------------------------------------------------------------
# Installation
#
include(GNUInstallDirs)

# executable
if(UNIX)
    set(DEFAULT_INSTALL_BIN_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}")
else()
    set(DEFAULT_INSTALL_BIN_DIR "${CMAKE_INSTALL_PREFIX}") 
endif()
set(INSTALL_BIN_DIR "${DEFAULT_INSTALL_BIN_DIR}" CACHE PATH "Installation directory for executable")
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION "${INSTALL_BIN_DIR}")

# data archive and readme
if(UNIX)
    set(DEFAULT_INSTALL_DATA_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}")
else()
    set(DEFAULT_INSTALL_DATA_DIR "${CMAKE_INSTALL_PREFIX}") 
endif()
set(INSTALL_DATA_DIR "${DEFAULT_INSTALL_DATA_DIR}" CACHE PATH "Installation directory for game data")
install(FILES ${PROJECT_ROOT_DIR}/game/data.zip DESTINATION "${INSTALL_DATA_DIR}")
install(FILES ${PROJECT_ROOT_DIR}/README.md DESTINATION "${INSTALL_DATA_DIR}")

if(UNIX)
    # man pages
    set(INSTALL_MAN_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_MANDIR}/man6" CACHE PATH "Installation directory for manual pages")
    install(FILES ${PROJECT_ROOT_DIR}/assets/docs/xrick.6.gz DESTINATION "${INSTALL_MAN_DIR}")
elseif(WIN32)
    # windows DLLs
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION "${DEFAULT_INSTALL_BIN_DIR}")
    include(InstallRequiredSystemLibraries)
endif()

#-----------------------------------------------------------------------------
# Create package
#
if(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
    set(CPACK_SET_DESTDIR true)
    set(CPACK_STRIP_FILES true)
    set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
    set(CPACK_PACKAGE_VERSION "${XRICK_VERSION_STR}")
    if(MSVC)
        if(CMAKE_CL_64)
            set(CPACK_PACKAGE_ARCH_SUFFIX "x86_64")
        else()
            set(CPACK_PACKAGE_ARCH_SUFFIX "x86")
        endif() 
    else()
        set(CPACK_PACKAGE_ARCH_SUFFIX ${CMAKE_SYSTEM_PROCESSOR})
    endif()
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "xrick - clone of the platform game Rick Dangerous")
    set(CPACK_PACKAGE_CONTACT "Pierluigi Vicinanza <pierluigiDOTvicinanzaDELETEME@gmail.com>")
    set(CPACK_RESOURCE_FILE_README ${PROJECT_ROOT_DIR}/README.md)
    
    # Deb package
    if(CPACK_GENERATOR MATCHES "DEB")
        if(CPACK_PACKAGE_ARCH_SUFFIX MATCHES "x86_64")
            set(CPACK_PACKAGE_ARCH_SUFFIX "amd64")
        else()
            set(CPACK_PACKAGE_ARCH_SUFFIX "i386")
        endif()
        set(CPACK_DEBIAN_ARCHITECTURE ${CPACK_PACKAGE_ARCH_SUFFIX})
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "libsdl1.2debian, zlib1g, libc6 (>= 2.1.3)")
        set(CPACK_DEBIAN_PACKAGE_SECTION "Games")
    endif()
    
    include(CPack)
endif()
