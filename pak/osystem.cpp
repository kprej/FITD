#include "osystem.h"
#include "bgfxHandle.h"

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
    ~private_t () = default;

    private_t ()
        : frameStart (0)
        , lastFrame (0)
        , startOfRender (nullptr)
        , endOfRender (nullptr)
        , mainThread (nullptr)
        , bgfxHandle ()
    {
    }

    unsigned long frameStart;
    unsigned long lastFrame;

    SDL_Semaphore *startOfRender;
    SDL_Semaphore *endOfRender;

    SDL_Thread *mainThread;

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

    m_d->startOfRender = SDL_CreateSemaphore (0);
    m_d->endOfRender = SDL_CreateSemaphore (0);

    printVersion ();
    PLOGD << "Init SDL";
    if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK) !=
        0)
    {
        PLOGF << SDL_GetError ();
    }

    GS ()->window = SDL_CreateWindow (
        "FITD", 1280, 960, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);

    if (!GS ()->window)
    {
        PLOGF << SDL_GetError ();
    }

    detectGame ();

    m_d->bgfxHandle.init ();
    // m_d->mainThread = SDL_CreateThread (FitdMain, "FitdMainThread", NULL);

    m_d->frameStart = SDL_GetTicks ();
    m_d->lastFrame = m_d->frameStart;
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
