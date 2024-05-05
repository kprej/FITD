#include "types.h"

std::string toString (gameId_t id_)
{
    switch (id_)
    {
    case gameId_t::AITD1:
        return "Alone in the Dark";
    case gameId_t::AITD2:
        return "Alone in the Dark 2";
    case gameId_t::AITD3:
        return "Alone in the Dark 3";
    case gameId_t::JACK:
        return "Jack in the Dark";
    case gameId_t::TIMEGATE:
        return "Time Gate";
    }

    return {};
}
