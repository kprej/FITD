#include "music.h"

#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Init.h>
#include <plog/Log.h>

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <filesystem>
using namespace std;

int main (int argc_, char *argv_[])
{
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init (plog::debug, &consoleAppender);
    filesystem::current_path (filesystem::path (argv_[0]).parent_path ());

    if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        PLOGF << SDL_GetError ();
    }
    auto window = SDL_CreateWindow ("ADLIB In The DARK",
                                    180,
                                    80,
                                    SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);

    if (!window)
    {
        PLOGF << SDL_GetError ();
        return 1;
    }

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

    PLOGD << initMusicDriver ();

    Mix_HookMusic (musicUpdate, NULL);

    setFile (pakFile_t (filesystem::path ("LISTMUS.PAK")));

    playMusic (0);

    while (true)
    {
    }

    return 0;
}
