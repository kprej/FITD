#pragma once

#include <cstdint>

struct channelTable2Element
{
    uint16_t index;
    struct channelTable2Element *var2;
    uint16_t var4;
    uint8_t *dataPtr;
    uint8_t *commandPtr;
    int16_t varE;
    uint16_t var10;
    uint8_t var12;
    uint16_t var13;
    uint16_t var15;
    uint8_t var17;
    uint16_t var18;
    uint8_t gain;
    uint8_t var1B;
    uint8_t var1C;
    uint8_t var1D;
    uint8_t var1E;
};

struct channelTableElement
{
    uint16_t var0;
    uint16_t var2;
    uint8_t var4;
    uint8_t var5;
    uint8_t var6;
    uint8_t var7;
    uint16_t var8;
};

extern channelTable2Element channelTable3[11];
extern channelTable2Element channelTable2[11];
extern unsigned char channelTableMelodic[];
extern unsigned char channelTableRythme[];
extern unsigned char *channelTable;

extern unsigned char regBDConf;

extern int musicStart ();
extern int musicLoad (uint8_t *musicPtr_);
extern int fadeMusic (int cx_, int si_, int dx_);
extern int update ();

extern void createDefaultChannel (int index);
extern void sendAdlib (int regIdx, int value);
extern void resetChannelFrequency (int channelIdx);
extern void setupChannelFrequency (int channelIdx, int cl, int dx, int bp);
