#pragma once
#include "bgfxHandle.h"
#include "game.h"
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
    std::map<std::string, pakFile_t> paks;

    std::unique_ptr<game_t> game;

    bgfxHandle_t handle;

    uint64_t delta;
};

struct input_t
{
    bool anyKey;

    void reset () { anyKey = false; }
};

class osystem_t
{
public:
    ~osystem_t ();
    osystem_t ();

    static std::shared_ptr<gameState_t> GS ();
    static std::shared_ptr<input_t> IN ();

    void init (int argc, char *argv[]);

    bool run ();

private:
    void detectGame ();
    bool handleInput ();
    void shutdown ();
    void loadPaks ();

    class private_t;
    std::unique_ptr<private_t> m_d;
};

inline std::shared_ptr<gameState_t> GS ()
{
    return osystem_t::GS ();
}

inline std::shared_ptr<input_t> IN ()
{
    return osystem_t::IN ();
}
