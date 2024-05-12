#pragma once

#include "pakFile.h"

#include <cstdint>

extern int nextMusic;
extern unsigned int musicChrono;
extern short int currentMusic;
extern pakFile_t musicPak;

int initMusicDriver (void);
void setFile (pakFile_t const &pak_);
void musicUpdate (void *udata, uint8_t *stream, int len);
void playMusic (int musicNumber);
void callMusicUpdate (void);
void destroyMusicDriver (void);
int fadeMusic (int param1, int param2, int param3);
