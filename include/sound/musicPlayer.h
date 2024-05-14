#pragma once

#include "pakFile.h"

#include <cstdint>
#include <memory>

extern void musicEnd ();

class musicPlayer_t
{
public:
    enum state_t
    {
        STOPPED,
        PLAYING,
        PAUSED
    };

    ~musicPlayer_t ();
    static std::shared_ptr<musicPlayer_t> PTR ();

    bool init ();
    bool isInit () const;

    void setMusicPak (pakFile_t const &file_);
    void setSpeed (uint16_t speed_);

    void playTrack (uint8_t trackNumber_);
    void setLoops (int8_t loops_);

    void shutdown ();

    void play ();
    void pause ();
    void stop ();

    void debug ();

private:
    musicPlayer_t ();

    friend void musicUpdate (void *udata, uint8_t *stream, int len);
    friend void musicEnd ();

    class private_t;
    std::unique_ptr<private_t> m_d;
};
