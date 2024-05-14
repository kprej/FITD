#pragma once

#include <string>
#include <vector>

namespace aitd
{
enum mus_t
{
    EXPLORING_DECERTO = 0,
    DANGER = 1,
    COMBAT = 2,
    CHOPIN_OPUS_69 = 3,
    BEAUTIFUL_DANUBE = 4,
    DANCE_OF_DEATH = 5,
    ENDING = 6,
    INTRO = 7,
};

extern std::vector<std::string> const TRACK_NAMES;
} // namespace aitd
