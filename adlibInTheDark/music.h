#pragma once

#include "pakFile.h"

#include <cstdint>

void musicUpdate (void *userData_, uint8_t *stream_, int len_);
void musicEnd ();

struct musicPlayer_t
{

    void callMusicUpdate ();
    bool init ();
    void destroyMusicDriver ();
    int fadeMusic (int param1, int param2, int param3);
    int update ();

    int musicLoad ();
    int musicStart ();

    bool PLAYING;

    int timeBeforeNextUpdate;
    int musicTimer = 0;
    int len;

    int fillStatus;

    uint8_t volume = 100;
    uint8_t currentTrack;

    pakFile_t musicPak;
    int musicSync = 9000;
    int nextUpdateTimer = musicSync;
    uint8_t *musicPtr;
    bool isOPLInit;
    uint64_t remaining;

    void playTrack (uint8_t trackNumber_);
};
