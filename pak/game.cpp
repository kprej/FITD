#include "game.h"

game_t::~game_t ()
{
}

game_t::game_t ()
{
}

void game_t::start ()
{
    _start ();
}

void game_t::readBook (int index_, int type_)
{
    _readBook (index_, type_);
}

void game_t::_start ()
{
    // no-op
}

void game_t::_readBook (int index_, int type_)
{
    // no-op
    (void)index_;
    (void)type_;
}
