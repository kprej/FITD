#pragma once

#include <cstdint>

class game_t
{
public:
    virtual ~game_t () = 0;
    game_t ();

    void init ();

    void start ();

    void readBook (int index_, int type_);

private:
    virtual void _init ();
    virtual void _start ();
    virtual void _readBook (int index_, int type_);
};
