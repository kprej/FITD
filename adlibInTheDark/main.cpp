#include "music.h"

#include "fmopl.h"
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

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

    SDL_SetHint (SDL_HINT_IME_SHOW_UI, "1");
    auto window = SDL_CreateWindow ("ADLIB In The DARK",
                                    1280,
                                    720,
                                    SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    int i, num_devices;
    SDL_AudioDeviceID *devices = SDL_GetAudioOutputDevices (&num_devices);

    if (!window)
    {
        PLOGF << SDL_GetError ();
        return 1;
    }

    auto renderer = SDL_CreateRenderer (window, NULL, 0);
    if (!renderer)
        return 1;

    ImGui::CreateContext ();

    if (!ImGui_ImplSDL3_InitForSDLRenderer (window, renderer))
    {
        PLOGF << "Failed to init ImGui";
        return 1;
    }
    ImGui_ImplSDLRenderer3_Init (renderer);

    SDL_AudioSpec spec;

    /* Initialize variables */
    spec.freq = MIX_DEFAULT_FREQUENCY;
    spec.format = MIX_DEFAULT_FORMAT;
    spec.channels = MIX_DEFAULT_CHANNELS;

    /* Open the audio device */
    musicPlayer_t *player = new musicPlayer_t ();

    ImVec4 clear_color = ImVec4 (0.45f, 0.55f, 0.60f, 1.00f);

    bool init = false;

    while (true)
    {
        ImGui_ImplSDLRenderer3_NewFrame ();
        ImGui_ImplSDL3_NewFrame ();
        ImGui::NewFrame ();

        SDL_Event event;

        while (SDL_PollEvent (&event))
        {
            ImGui_ImplSDL3_ProcessEvent (&event);
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                return 0;
            }
        }

        if (ImGui::BeginMainMenuBar ())
        {
            ImGui::Text (" %.2f FPS (%.2f ms)",
                         ImGui::GetIO ().Framerate,
                         1000.0f / ImGui::GetIO ().Framerate);

            ImGui::EndMainMenuBar ();
        }

        if (!init)
        {
            if (ImGui::Begin ("Init"))
            {
                int8_t ret = -1;
                for (i = 0; i < num_devices; ++i)
                {
                    SDL_AudioDeviceID instance_id = devices[i];
                    string name = SDL_GetAudioDeviceName (instance_id);
                    if (ImGui::Button (name.c_str ()))
                        ret = Mix_OpenAudio (instance_id, &spec);
                }
                if (ret != -1)
                {
                    Mix_QuerySpec (&spec.freq, &spec.format, &spec.channels);
                    PLOGD << "Opened audio at " << spec.freq << " Hz "
                          << (spec.format & 0xFF) << "bit";

                    player->musicPak = pakFile_t (filesystem::path ("LISTMUS.PAK"));
                    switch (spec.format)
                    {
                    case SDL_AUDIO_S16:
                        player->musicSync = 3000;
                        break;
                    case SDL_AUDIO_S32:
                        player->musicSync = 6000;
                        break;
                    };

                    Mix_HookMusic (musicUpdate, player);
                    Mix_HookMusicFinished (musicEnd);

                    player->init ();
                    init = true;
                }
                ImGui::End ();
            }
        }
        else if (init)
        {
            if (ImGui::Begin ("Sound"))
            {
                int i = 0;
                for (auto const &pak : player->musicPak.paks ())
                {
                    if (ImGui::Button (("Song " + to_string (i)).c_str ()))
                    {
                        player->playTrack (i);
                    }
                    ++i;
                }

                int volume = player->volume;
                ImGui::SliderInt ("Volume", &volume, 0, 124);
                if (player->volume != volume)
                {
                    player->volume = volume;
                    player->fadeMusic (volume, 0, 0x80);
                }

                ImGui::End ();
            }
        }

        ImGui::Render ();
        SDL_SetRenderDrawColor (renderer,
                                (Uint8)(clear_color.x * 255),
                                (Uint8)(clear_color.y * 255),
                                (Uint8)(clear_color.z * 255),
                                (Uint8)(clear_color.w * 255));
        SDL_RenderClear (renderer);
        ImGui_ImplSDLRenderer3_RenderDrawData (ImGui::GetDrawData ());
        SDL_RenderPresent (renderer);
    }

    return 0;
}
