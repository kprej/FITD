#pragma once
#include "pakFile.h"
#include "types.h"

#include <SDL3/SDL.h>

#include <array>
#include <map>
#include <memory>
#include <string>
#include <vector>

struct gameState_t
{
    gameId_t gameId;
    bool debugMenuDisplayed;
    bool shutdown;
    std::vector<int16_t> CVars;
    int outputResolution[2];
    SDL_Window *window;

    std::array<std::byte, 768> currentPalette;

    std::map<std::string, pak_t> paks;
};

class osystem_t
{
public:
    ~osystem_t ();
    osystem_t ();

    static std::shared_ptr<gameState_t> GS ();

    void init (int argc, char *argv[]);

    bool run ();

private:
    void detectGame ();
    bool handleInput ();
    void shutdown ();

    class private_t;
    std::unique_ptr<private_t> m_d;
};

inline std::shared_ptr<gameState_t> GS ()
{
    return osystem_t::GS ();
}
