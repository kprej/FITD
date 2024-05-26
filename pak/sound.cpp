#include "sound.h"

#include <SDL3/SDL_iostream.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <plog/Log.h>

using namespace std;

class sound_t::private_t
{
public:
    ~private_t () { Mix_FreeChunk (chunk); }
    private_t ()
        : chunk (nullptr)
        , currentChannel (-1)
    {
    }

    Mix_Chunk *chunk;

    int currentChannel;
};

sound_t::~sound_t () = default;
sound_t::sound_t ()
    : m_d (make_shared<private_t> ())
{
}

void sound_t::init (vector<byte> const &data_)
{
    SDL_IOStream *stream = SDL_IOFromConstMem (data_.data (), data_.size ());

    m_d->chunk = Mix_LoadWAV_IO (stream, true);
}

int sound_t::play () const
{
    m_d->currentChannel = Mix_PlayChannel (-1, m_d->chunk, 0);
    return m_d->currentChannel;
}

bool sound_t::isPlaying () const
{
    return Mix_Playing (m_d->currentChannel);
}
