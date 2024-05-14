#include "musicPlayer.h"
#include "fmopl.h"
#include "musicBackend.h"

#include <imgui.h>

#include <SDL3_mixer/SDL_mixer.h>
#include <plog/Log.h>

using namespace std;

#define OPL_INTERNAL_FREQ 3579545

class musicPlayer_t::private_t
{
public:
    ~private_t () = default;
    private_t () = default;

    state_t state;

    int timeBeforeNextUpdate;
    int musicTimer = 0;
    int len;

    int fillStatus;

    uint8_t volume = 100;
    uint8_t currentTrack = 255;
    int loops = 0;

    pakFile_t musicPak;
    int musicSync = 9000;
    int nextUpdateTimer = musicSync;
    bool isOPLInit;
    uint64_t remaining;
};

void musicUpdate (void *udata, uint8_t *stream, int len)
{
    auto player = musicPlayer_t::PTR ();
    if (true)
    {
        player->m_d->fillStatus = 0;
        player->m_d->len = len;

        while (player->m_d->fillStatus < player->m_d->len)
        {
            player->m_d->timeBeforeNextUpdate =
                player->m_d->nextUpdateTimer - player->m_d->musicTimer;

            if (player->m_d->timeBeforeNextUpdate >
                (player->m_d->len - player->m_d->fillStatus))
            {
                player->m_d->timeBeforeNextUpdate =
                    player->m_d->len - player->m_d->fillStatus;
            }

            if (player->m_d->timeBeforeNextUpdate) // generate
            {
                YM3812UpdateOne (0,
                                 (int16_t *)(stream + player->m_d->fillStatus),
                                 (player->m_d->timeBeforeNextUpdate) / 2);
                player->m_d->fillStatus += player->m_d->timeBeforeNextUpdate;
                player->m_d->musicTimer += player->m_d->timeBeforeNextUpdate;
                player->m_d->remaining -= 1;
            }

            if (player->m_d->musicTimer == player->m_d->nextUpdateTimer)
            {
                update ();

                player->m_d->nextUpdateTimer += player->m_d->musicSync;
            }
        }
    }
}

void musicEnd ()
{
    auto player = musicPlayer_t::PTR ();
    if (player->m_d->state == musicPlayer_t::STOPPED)
        return;

    if (player->m_d->loops == 0)
    {
        player->m_d->state = musicPlayer_t::STOPPED;
        return;
    }

    if (player->m_d->loops == -1)
    {
        player->m_d->state = musicPlayer_t::PLAYING;
        fadeMusic (player->m_d->volume, 0, 0x80);
        return;
    }

    player->m_d->state = musicPlayer_t::PLAYING;
    fadeMusic (player->m_d->volume, 0, 0x80);

    --player->m_d->loops;
}

shared_ptr<musicPlayer_t> musicPlayer_t::PTR ()
{
    static shared_ptr<musicPlayer_t> mp;

    if (mp)
        return mp;

    mp.reset (new musicPlayer_t ());

    return mp;
}
musicPlayer_t::~musicPlayer_t () = default;
musicPlayer_t::musicPlayer_t ()
    : m_d (make_unique<private_t> ())
{
}

bool musicPlayer_t::init ()
{
    if (m_d->isOPLInit)
        return true;

    int i;

    if (YM3812Init (1, OPL_INTERNAL_FREQ, 44100))
        return false;

    for (i = 0; i < 11; i++)
    {
        channelTable2[i].var4 |= 0x20;
        channelTable2[i].var2->var4 |= 0x20;

        createDefaultChannel (i);
    }

    m_d->isOPLInit = true;

    Mix_HookMusic (musicUpdate, nullptr);
    Mix_HookMusicFinished (musicEnd);
    return true;
}

bool musicPlayer_t::isInit () const
{
    return m_d->isOPLInit;
}

void musicPlayer_t::setMusicPak (pakFile_t const &file_)
{
    m_d->musicPak = file_;
}

void musicPlayer_t::setSpeed (uint16_t speed_)
{
    m_d->musicSync = speed_;
}

void musicPlayer_t::shutdown ()
{
    YM3812Shutdown ();
}

void musicPlayer_t::playTrack (uint8_t trackNumber_)
{
    if (m_d->currentTrack != trackNumber_ || m_d->state == STOPPED)
    {
        if (trackNumber_ >= 0 && trackNumber_ < m_d->musicPak.paks ().size ())
        {
            fadeMusic (0, 0, 0x40);
            m_d->state = STOPPED;

            musicLoad (m_d->musicPak.pak (trackNumber_).raw ());
            musicStart ();

            fadeMusic (m_d->volume, 0, 0x80);

            m_d->currentTrack = trackNumber_;
            m_d->state = PLAYING;
        }
    }
}

void musicPlayer_t::debug ()
{
    string extra = "";
    if (m_d->state != STOPPED)
        extra = " : Now Playing Track " + to_string (m_d->currentTrack);

    if (ImGui::Begin (("Music" + extra).c_str ()))
    {
        int i = 0;
        for (auto const &pak : m_d->musicPak.paks ())
        {
            if (ImGui::Button (("Track " + std::to_string (i)).c_str ()))
                playTrack (i);

            if (i % 2 == 0)
                ImGui::SameLine ();

            ++i;
        }

        int inputLoops = m_d->loops;
        ImGui::InputInt ("Loops", &inputLoops, -1, 100);
        if (m_d->loops != inputLoops)
        {
            m_d->loops = inputLoops;
        }
        ImGui::Text ("Active Channels: %i", ACTIVE_CHANNELS);

        float inputVolume = float (m_d->volume) / 124;

        ImGui::SliderFloat ("Volume", &inputVolume, 0.f, 1.f);
        if (m_d->volume != inputVolume * 124)
        {
            m_d->volume = inputVolume * 124;
            fadeMusic (m_d->volume, 0, 0x8000);
        }

        if (m_d->state == PLAYING && ImGui::Button ("PAUSE"))
            m_d->state = PAUSED;

        if (m_d->state == PAUSED && ImGui::Button ("RESUME"))
            m_d->state = PLAYING;

        ImGui::SameLine ();

        if (m_d->state == PLAYING && ImGui::Button ("STOP"))
        {
            m_d->state = STOPPED;
            fadeMusic (0, 0, 0x40);
        }
    }
    ImGui::End ();
}
