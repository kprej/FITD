#pragma once
#include "bgfxHandle.h"
#include "camera.h"
#include "debugHandle.h"
#include "font.h"
#include "game.h"
#include "musicPlayer.h"
#include "pakFile.h"
#include "sound.h"
#include "types.h"

#include <SDL3/SDL.h>

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
    font_t font;

    float width;
    float height;

    camera_t camera;

    bgfxHandle_t handle;
    debugHandle_t debug;

    uint64_t delta;

    std::vector<sound_t> samples;
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

    void setupAudio ();

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

inline std::shared_ptr<musicPlayer_t> MP ()
{
    return musicPlayer_t::PTR ();
}
