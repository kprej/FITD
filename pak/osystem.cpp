#include "osystem.h"
#include "AITD.h"
#include "bgfxHandle.h"

#include <backends/imgui_impl_sdl3.h>
#include <bgfx/bgfx.h>

#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Init.h>
#include <plog/Log.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_mutex.h>

#include <filesystem>
using namespace std;

#define printVersion() PLOGD << "Compiled the " << __DATE__ << " at " << __TIME__;

class osystem_t::private_t
{
public:
    ~private_t () {}

    private_t ()
        : frameStart (0)
        , lastFrame (0)
    {
    }

    unsigned long frameStart;
    unsigned long lastFrame;
};

shared_ptr<gameState_t> osystem_t::GS ()
{
    static shared_ptr<gameState_t> gs;

    if (gs)
        return gs;

    gs = make_shared<gameState_t> ();

    return gs;
}

osystem_t::~osystem_t () = default;

osystem_t::osystem_t ()
    : m_d (make_unique<private_t> ())
{
}

void osystem_t::init (int argc_, char *argv_[])
{
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init (plog::debug, &consoleAppender);

    filesystem::current_path (filesystem::path (argv_[0]).parent_path ());

    printVersion ();

    if constexpr (std::endian::native == std::endian::big)
        PLOGD << "Big Endian";
    else if constexpr (std::endian::native == std::endian::little)
        PLOGD << "Little Endian";
    else
        PLOGD << "Mixed Endian";

    PLOGD << "Init SDL";
    if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK) !=
        0)
    {
        PLOGF << SDL_GetError ();
    }

    SDL_SetHint (SDL_HINT_IME_SHOW_UI, "1");

    loadPaks ();
    detectGame ();

    GS ()->handle.init ();

    m_d->frameStart = SDL_GetTicks ();
    m_d->lastFrame = m_d->frameStart;
}

bool osystem_t::run ()
{
    if (!handleInput ())
        return false;

    GS ()->handle.startFrame ();
    GS ()->game->start ();
    GS ()->handle.endFrame ();

    return true;
}

void osystem_t::detectGame ()
{
    if (filesystem::exists ("LISTBOD2.PAK"))
    {
        PLOGD << "Detected Alone in the Dark";

        GS ()->gameId = gameId_t::AITD1;
        GS ()->CVars.resize (45);
        // currentCVarTable = AITD1Knownm_d->CVars;

        GS ()->game = make_unique<aitd_t> ();

        return;
    }
    else if (filesystem::exists ("PERE.PAK"))
    {
        PLOGD << "Detected Jack in the Dark";

        GS ()->gameId = gameId_t::JACK;
        GS ()->CVars.resize (70);
        // currentCVarTable = AITD2KnownGS ()->CVars;

        return;
    }
    else if (filesystem::exists ("MER.PAK"))
    {
        PLOGD << "Detected Alone in the Dark 2";

        GS ()->gameId = gameId_t::AITD2;
        GS ()->CVars.resize (70);
        // currentCVarTable = AITD2KnownGS ()->CVars;

        return;
    }
    else if (filesystem::exists ("AN1.PAK"))
    {
        PLOGD << "Detected Alone in the Dark 3";

        GS ()->gameId = gameId_t::AITD3;
        GS ()->CVars.resize (70);
        // currentCVarTable = AITD2KnownGS ()->CVars;

        return;
    }
    else if (filesystem::exists ("PURSUIT.PAK"))
    {
        PLOGD << "Detected Time Gate";

        GS ()->gameId = gameId_t::TIMEGATE;
        GS ()->CVars.resize (100); // TODO: figure this
        // currentCVarTable = AITD2KnownGS ()->CVars; // TODO: figure this

        return;
    }

    PLOGF << "Game detection failed...";
}

bool osystem_t::handleInput ()
{
    SDL_Event event;

    while (SDL_PollEvent (&event))
    {
        ImGui_ImplSDL3_ProcessEvent (&event);

        switch (event.type)
        {
        case SDL_EVENT_KEY_DOWN:
            if (event.key.keysym.scancode == SDL_SCANCODE_GRAVE)
                GS ()->debugMenuDisplayed = !GS ()->debugMenuDisplayed;
            break;
        case SDL_EVENT_QUIT:
            shutdown ();
            return false;
        }
    }

    return true;
}

void osystem_t::shutdown ()
{
    PLOGD << "Begin shutdown event";
    GS ()->handle.shutdown ();

    PLOGD << "Shutdown SDL";
    SDL_Quit ();
}

void osystem_t::loadPaks ()
{
    for (auto const &file : filesystem::directory_iterator {filesystem::current_path ()})
    {
        if (file.path ().extension () != ".PAK")
            continue;

        GS ()->paks.insert ({file.path ().stem ().string (), pak_t (file.path ())});
    }
}
