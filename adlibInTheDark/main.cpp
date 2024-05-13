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
    spec.format = SDL_AUDIO_S32;
    spec.channels = MIX_DEFAULT_CHANNELS;

    /* Open the audio device */
    auto ret = Mix_OpenAudio (0, &spec);
    if (ret < 0)
    {
        PLOGF << "Couldn't open audio: " << SDL_GetError ();
        return 1;
    }
    else
    {
        Mix_QuerySpec (&spec.freq, &spec.format, &spec.channels);
        PLOGD << "Opened audio at " << spec.freq << " Hz " << (spec.format & 0xFF)
              << "bit";
    }

    musicPlayer_t *player = new musicPlayer_t ();
    player->musicPak = pakFile_t (filesystem::path ("LISTMUS.PAK"));

    Mix_HookMusic (musicUpdate, player);
    Mix_HookMusicFinished (musicEnd);

    player->init ();

    ImVec4 clear_color = ImVec4 (0.45f, 0.55f, 0.60f, 1.00f);

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
            ImGui::InputInt ("Volume", &volume, 0, 100);
            player->volume = volume;

            ImGui::Text ("Playing %i\n", player->PLAYING);
            ImGui::Text ("Remaining %li\n", player->remaining);
            ImGui::Text ("Music Sync %i\n", player->musicSync);
            ImGui::Text ("Music Timer %i\n", player->musicTimer);
            ImGui::Text ("FillStatus %i\n", player->fillStatus);
            ImGui::Text ("Len %i\n", player->len);
            ImGui::Text ("Time Before Next Update %i\n", player->timeBeforeNextUpdate);
            ImGui::End ();
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
