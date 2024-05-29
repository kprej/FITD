#pragma once

#include <cstdint>

enum resources_t : uint8_t
{
    FRAME_TOP_LEFT = 0,
    FRAME_TOP_RIGHT = 1,
    FRAME_BOT_LEFT = 2,
    FRAME_BOT_RIGHT = 3,
    FRAME_TOP,
    FRAME_BOT,
    FRAME_LEFT,
    FRAME_RIGHT,
    FRAME_ITD,

    TEX_TATOU,
    PAL_TATOU,
    PAL_GAME = 255,
};
