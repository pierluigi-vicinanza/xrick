#
# data_extractor/projects/cmake/CMakeLists.txt
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

project(data_extractor C)

set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../..)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif(NOT CMAKE_BUILD_TYPE)
message(STATUS ${PROJECT_NAME} " build type: " ${CMAKE_BUILD_TYPE})

#-----------------------------------------------------------------------------
# Check for configuration file
#
if(NOT EXISTS ${PROJECT_ROOT_DIR}/source/xrick/config.h)
    message(FATAL_ERROR
            "Could not find xrick configuration file in " 
            ${PROJECT_ROOT_DIR}/source/xrick/config.h "\n"
            "Please run CMake on xrick project to generate it.\n")
endif()

#-----------------------------------------------------------------------------
# Source files
#
set(SOURCES
    ${PROJECT_ROOT_DIR}/source/data_extractor/dat_ents.c
    ${PROJECT_ROOT_DIR}/source/data_extractor/dat_ents.h
    ${PROJECT_ROOT_DIR}/source/data_extractor/dat_maps.c
    ${PROJECT_ROOT_DIR}/source/data_extractor/dat_maps.h
    ${PROJECT_ROOT_DIR}/source/data_extractor/dat_pics.h
    ${PROJECT_ROOT_DIR}/source/data_extractor/dat_picsPC.c
    ${PROJECT_ROOT_DIR}/source/data_extractor/dat_picsST.c
    ${PROJECT_ROOT_DIR}/source/data_extractor/dat_screens.c
    ${PROJECT_ROOT_DIR}/source/data_extractor/dat_screens.h
    ${PROJECT_ROOT_DIR}/source/data_extractor/dat_sprites.h
    ${PROJECT_ROOT_DIR}/source/data_extractor/dat_spritesPC.c
    ${PROJECT_ROOT_DIR}/source/data_extractor/dat_spritesST.c
    ${PROJECT_ROOT_DIR}/source/data_extractor/dat_tiles.h
    ${PROJECT_ROOT_DIR}/source/data_extractor/dat_tilesPC.c
    ${PROJECT_ROOT_DIR}/source/data_extractor/dat_tilesST.c
    ${PROJECT_ROOT_DIR}/source/data_extractor/main.c
    ${PROJECT_ROOT_DIR}/source/xrick/config.h
    ${PROJECT_ROOT_DIR}/source/xrick/res_magic.c
    ${PROJECT_ROOT_DIR}/source/xrick/resources.h
    ${PROJECT_ROOT_DIR}/source/xrick/data/pics.h
    ${PROJECT_ROOT_DIR}/source/xrick/data/img.h
    ${PROJECT_ROOT_DIR}/source/xrick/system/basic_funcs.c
    ${PROJECT_ROOT_DIR}/source/xrick/system/basic_funcs.h
    ${PROJECT_ROOT_DIR}/source/xrick/system/basic_types.h
    ${PROJECT_ROOT_DIR}/source/xrick/system/miniz_config.h
)

#-----------------------------------------------------------------------------
# Create target
#
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE
                           ${PROJECT_ROOT_DIR}/source
                           ${PROJECT_ROOT_DIR}/source/xrick/3rd_party)

if(CMAKE_COMPILER_IS_GNUCC)
    set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-std=gnu99")
endif()

if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS "_CRT_SECURE_NO_WARNINGS")
endif()

#-----------------------------------------------------------------------------
# Copy generated binary to bin folder and create required subfolders
#
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/bin
                   COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/bin/graphics
                   COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/bin/misc
                   COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/bin/texts
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_BINARY_DIR}/bin)
