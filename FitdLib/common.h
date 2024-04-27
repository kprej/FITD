#ifndef _COMMON_H_
#define _COMMON_H_

#include "config.h"

#ifdef USE_IMGUI
#include "imgui.h"
#endif

//////////////// GAME SPECIFIC DEFINES

#define NUM_MAX_CAMERA_IN_ROOM 20
// #define NUM_MAX_OBJ         300
#define NUM_MAX_OBJECT 50
#define NUM_MAX_TEXT 40
#define NUM_MAX_MESSAGE 5

// 250
#define NUM_MAX_TEXT_ENTRY 1000

//////////////////

extern int AITD1KnownCVars[];
extern int AITD2KnownCVars[];
extern int *currentCVarTable;

// int getCVarsIdx (enumCVars);
int getCVarsIdx (int);

//////////////////////

#define SAMPLE_PAGE 0
#define BODY_FLAMME 1
#define MAX_WEIGHT_LOADABLE 2
#define TEXTE_CREDITS 3
#define SAMPLE_TONNERRE 4
#define INTRO_DETECTIVE 5
#define INTRO_HERITIERE 6
#define WORLD_NUM_PERSO 7
#define CHOOSE_PERSO 8
#define SAMPLE_CHOC 9
#define SAMPLE_PLOUF 10
#define REVERSE_OBJECT 11
#define KILLED_SORCERER 12
#define LIGHT_OBJECT 13
#define FOG_FLAG 14
#define DEAD_PERSO 15

//////////////////

#if !AITD_UE4
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
#ifndef UNIX
typedef unsigned int uint;
#endif
typedef signed char int8;
typedef signed short int16;
typedef signed long int32;
#endif

typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned long U32;
typedef signed char S8;
typedef signed short S16;
typedef signed long S32;

#define TYPE_MASK 0x1D1

#define ANIM_ONCE 0
#define ANIM_REPEAT 1
#define ANIM_UNINTERRUPTABLE 2
#define ANIM_RESET 4

#include "actorList.h"
#include "aitdBox.h"
#include "anim.h"
#include "animAction.h"
#include "cosTable.h"
#include "fileAccess.h"
#include "floor.h"
#include "fmopl.h"
#include "font.h"
#include "gameTime.h"
#include "hqr.h"
#include "input.h"
#include "inventory.h"
#include "main.h"
#include "mainLoop.h"
#include "music.h"
#include "object.h"
#include "pak.h"
#include "renderer.h"
#include "room.h"
#include "save.h"
#include "screen.h"
#include "sequence.h"
#include "startupMenu.h"
#include "systemMenu.h"
#include "tatou.h"
#include "threadCode.h"
#include "unpack.h"
#include "vars.h"
#include "version.h"
#include "videoMode.h"
#include "zv.h"

// include game specific stuff
#include "AITD1.h"
#include "AITD2.h"
#include "AITD3.h"
#include "JACK.h"

// debugger
#ifdef FITD_DEBUGGER
#include "debugger.h"
#endif

// scripting
#include "evalVar.h"
#include "life.h"
#include "track.h"

#include "osystem.h"

////

// typedef unsigned char byte;

#ifdef UNIX
#define FORCEINLINE static inline
#else
#ifdef WIN32
#define FORCEINLINE __forceinline
#else
#define FORCEINLINE inline
#endif
#endif

FORCEINLINE u8 READ_LE_U8 (void *ptr)
{
    return *(u8 *)ptr;
}

FORCEINLINE s8 READ_LE_S8 (void *ptr)
{
    return *(s8 *)ptr;
}

FORCEINLINE u16 READ_LE_U16 (void *ptr)
{
#ifdef MACOSX
    return (((u8 *)ptr)[1] << 8) | ((u8 *)ptr)[0];
#else
    return *(u16 *)ptr;
#endif
}

FORCEINLINE s16 READ_LE_S16 (void *ptr)
{
    return (s16)READ_LE_U16 (ptr);
}

FORCEINLINE u16 READ_BE_U16 (void *ptr)
{
#ifdef MACOSX
    return *(u16 *)ptr;
#else
    return (((u8 *)ptr)[1] << 8) | ((u8 *)ptr)[0];
#endif
}

FORCEINLINE s16 READ_BE_S16 (void *ptr)
{
    return (s16)READ_BE_S16 (ptr);
}

FORCEINLINE u32 READ_LE_U32 (void *ptr)
{
#ifdef MACOSX
    return (((u8 *)ptr)[3] << 24) | (((u8 *)ptr)[2] << 16) | (((u8 *)ptr)[1] << 8) |
           ((u8 *)ptr)[0];
#else
    return *(u32 *)ptr;
#endif
}

FORCEINLINE s32 READ_LE_S32 (void *ptr)
{
    return (s32)READ_LE_U32 (ptr);
}

FORCEINLINE u32 READ_BE_U32 (void *ptr)
{
#ifdef MACOSX
    return *(u32 *)ptr;
#else
    return (((u8 *)ptr)[3] << 24) | (((u8 *)ptr)[2] << 16) | (((u8 *)ptr)[1] << 8) |
           ((u8 *)ptr)[0];
#endif
}

FORCEINLINE s32 READ_BE_S32 (void *ptr)
{
    return (s32)READ_LE_U32 (ptr);
}

#endif
