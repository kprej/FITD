#pragma once

#include "pakFile.h"

#include <cstdint>

extern bool PLAYING;
extern int nextMusic;
extern unsigned int musicChrono;
extern short int currentMusic;
extern pakFile_t musicPak;
extern int musicSync;

int initMusicDriver (void);
void setFile (pakFile_t const &pak_);
void musicUpdate (void *udata, uint8_t *stream, int len);
void callMusicUpdate (void);
void destroyMusicDriver (void);
int fadeMusic (int param1, int param2, int param3);

struct musicPlayer_t
{
    bool PLAYING;

    int timeBeforeNextUpdate;
    int nextUpdateTimer;
    int musicTimer;
    int len;

    int fillStatus;

    unsigned int musicChrono;
    short int currentMusic;
    pakFile_t musicPak;
    int musicSync;
    uint8_t *musicPtr;

    void playMusic (int musicNumber);
};
