#pragma once

#include <SDL.h>

void createBgfxInitParams ();
void deleteBgfxGlue ();

void StartFrame ();
void EndFrame ();

extern int gFrameLimit;
extern bool gCloseApp;

extern SDL_Window *gWindowBGFX;
