#include "osystem.h"
#include "bgfxHandle.h"

#include "AITD/AITD.h"

#include <backends/imgui_impl_sdl3.h>
#include <bgfx/bgfx.h>

#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Init.h>
#include <plog/Log.h>

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

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

    uint64_t lastTime;
};

shared_ptr<gameState_t> osystem_t::GS ()
{
    static shared_ptr<gameState_t> gs;

    if (gs)
        return gs;

    gs = make_shared<gameState_t> ();

    return gs;
}

shared_ptr<input_t> osystem_t::IN ()
{
    static shared_ptr<input_t> in;

    if (in)
        return in;

    in = make_shared<input_t> ();

    return in;
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
    SDL_setenv ("SDL_AUDIO_DRIVER", AUDIO_DRIVER, 1);

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

    PLOGD << "Init SDL Mixer";
    setupAudio ();

    SDL_SetHint (SDL_HINT_IME_SHOW_UI, "1");
    m_d->lastTime = SDL_GetTicks ();

    GS ()->handle.init ();
    GS ()->camera.init ();

    loadPaks ();
    detectGame ();
    GS ()->handle.startFrame ();
    GS ()->handle.endFrame ();
}

bool osystem_t::run ()
{
    GS ()->delta = SDL_GetTicks () - m_d->lastTime;
    m_d->lastTime = SDL_GetTicks ();
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
        GS ()->game->init ();

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

    IN ()->reset ();

    while (SDL_PollEvent (&event))
    {
        ImGui_ImplSDL3_ProcessEvent (&event);

        switch (event.type)
        {
        case SDL_EVENT_KEY_DOWN:
            IN ()->anyKey = true;
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
    GS ()->game.reset (nullptr);
    GS ()->samples.clear ();

    PLOGD << "Begin shutdown event";
    GS ()->handle.shutdown ();

    PLOGD << "Shutdown SDL";
    Mix_CloseAudio ();
    SDL_Quit ();
}

void osystem_t::loadPaks ()
{
    for (auto const &file : filesystem::directory_iterator {filesystem::current_path ()})
    {
        if (file.path ().extension () != ".PAK")
            continue;

        GS ()->paks.insert ({file.path ().stem ().string (), pakFile_t (file.path ())});
    }
}

void osystem_t::setupAudio ()
{
    SDL_AudioSpec spec;

    /* Initialize variables */
    spec.freq = MIX_DEFAULT_FREQUENCY;
    spec.format = MIX_DEFAULT_FORMAT;
    spec.channels = MIX_DEFAULT_CHANNELS;

    /* Open the audio device */
    auto ret = Mix_OpenAudio (0, &spec);
    if (ret < 0)
    {
        PLOGF << "Couldn't open audio: " << SDL_GetError ();
    }
    else
    {
        Mix_QuerySpec (&spec.freq, &spec.format, &spec.channels);
        SDL_Log ("Opened audio at %d Hz %d bit%s %s",
                 spec.freq,
                 (spec.format & 0xFF),
                 (SDL_AUDIO_ISFLOAT (spec.format) ? " (float)" : ""),
                 (spec.channels > 2)   ? "surround"
                 : (spec.channels > 1) ? "stereo"
                                       : "mono");
    }
}
