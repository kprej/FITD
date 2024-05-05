#include "osystem.h"
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
        , bgfxHandle ()
    {
    }

    unsigned long frameStart;
    unsigned long lastFrame;

    bgfxHandle_t bgfxHandle;
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
    plog::init (plog::verbose, &consoleAppender);

    filesystem::current_path (filesystem::path (argv_[0]).parent_path ());

    printVersion ();
    PLOGD << "Init SDL";
    if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK) !=
        0)
    {
        PLOGF << SDL_GetError ();
    }

    SDL_SetHint (SDL_HINT_IME_SHOW_UI, "1");

    GS ()->window = SDL_CreateWindow (
        "FITD", 1280, 800, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);

    if (!GS ()->window)
    {
        PLOGF << SDL_GetError ();
    }

    detectGame ();

    m_d->bgfxHandle.init ();

    m_d->frameStart = SDL_GetTicks ();
    m_d->lastFrame = m_d->frameStart;
}

bool osystem_t::run ()
{
    if (!handleInput ())
        return false;

    m_d->bgfxHandle.startFrame ();

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

        SDL_SetWindowTitle (GS ()->window, "Alone in the Dark");
        return;
    }
    else if (filesystem::exists ("PERE.PAK"))
    {
        PLOGD << "Detected Jack in the Dark";

        GS ()->gameId = gameId_t::JACK;
        GS ()->CVars.resize (70);
        // currentCVarTable = AITD2KnownGS ()->CVars;

        SDL_SetWindowTitle (GS ()->window, "Jack in the Dark");
        return;
    }
    else if (filesystem::exists ("MER.PAK"))
    {
        PLOGD << "Detected Alone in the Dark 2";

        GS ()->gameId = gameId_t::AITD2;
        GS ()->CVars.resize (70);
        // currentCVarTable = AITD2KnownGS ()->CVars;

        SDL_SetWindowTitle (GS ()->window, "Alone in the Dark 2");
        return;
    }
    else if (filesystem::exists ("AN1.PAK"))
    {
        PLOGD << "Detected Alone in the Dark 3";

        GS ()->gameId = gameId_t::AITD3;
        GS ()->CVars.resize (70);
        // currentCVarTable = AITD2KnownGS ()->CVars;

        SDL_SetWindowTitle (GS ()->window, "Alone in the Dark 3");
        return;
    }
    else if (filesystem::exists ("PURSUIT.PAK"))
    {
        PLOGD << "Detected Time Gate";

        GS ()->gameId = gameId_t::TIMEGATE;
        GS ()->CVars.resize (100); // TODO: figure this
        // currentCVarTable = AITD2KnownGS ()->CVars; // TODO: figure this

        SDL_SetWindowTitle (GS ()->window, "Time Gate");
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
    m_d->bgfxHandle.shutdown ();

    PLOGD << "Shutdown SDL";
    SDL_DestroyWindow (GS ()->window);
    SDL_Quit ();
}
