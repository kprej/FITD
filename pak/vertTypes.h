#pragma once
#include <cstdint>

struct rawBody_t
{
    float pos[3];
};

struct primitive_t
{
    uint16_t start;
    uint8_t size;
    float color;
};

struct textureVert_t
{
    float position[3];
    float texcoord[2];
};
