#pragma once

#include <cstdint>
#include <vector>

int hqrKeyGen = 0;

struct hqrSubEntry_t
{
    int16_t key;
    int16_t size;
    unsigned int lastTimeUsed;
    char *ptr;
};

struct hqrEntry_t
{
    char string[10];
    uint16_t maxFreeData;
    uint16_t sizeFreeData;
    uint16_t numMaxEntry;
    uint16_t numUsedEntry;
    std::vector<hqrSubEntry_t> entries;
};

char *HQR_Get (hqrEntry_t *hqrPtr, int index);
int HQ_Malloc (hqrEntry_t *hqrPtr, int size);
char *HQ_PtrMalloc (hqrEntry_t *hqrPtr, int index);
hqrEntry_t *HQR_InitRessource (const char *name, int size, int numEntries);
hqrEntry_t *HQR_Init (int size, int numEntry);
void HQR_Reset (hqrEntry_t *hqrPtr);
void HQR_Free (hqrEntry_t *hqrPtr);

sBody *getBodyFromPtr (void *ptr);
sAnimation *getAnimationFromPtr (void *ptr);
