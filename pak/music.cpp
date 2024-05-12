#include "music.h"
#include "osystem.h"

#include <SDL3_mixer/SDL_mixer.h>
#include <adlmidi.h>

#include <plog/Log.h>

using namespace std;

class music_t::private_t
{
public:
    ~private_t () {}
    private_t ()
        : data ()
    {
    }
    vector<byte> data;
};

music_t::~music_t () = default;
music_t::music_t ()
    : m_d (make_shared<private_t> ())
{
}

void music_t::init (vector<byte> const &data_)
{
    m_d->data = data_;
}

void music_t::play () const
{
    if (adl_openData (GS ()->amDevice.get (), m_d->data.data (), m_d->data.size ()) < 0)
        PLOGF << "FICL";

    Mix_PauseAudio (0);
}
