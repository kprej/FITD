#pragma once

#include "frame.h"

#include <bgfx/bgfx.h>

using namespace std;

class frame_t::private_t
{
public:
    ~private_t () {}
    private_t () {}

    bgfx::TextureHandle topLeftTexture;
    bgfx::TextureHandle topRightTexture;

    bgfx::TextureHandle topTexture;
    bgfx::TextureHandle leftTexture;
    bgfx::TextureHandle rightTexture;
    bgfx::TextureHandle bottomTexture;
    bgfx::TextureHandle inTheDarkTexture;
};

frame_t::~frame_t () = default;

frame_t::frame_t ()
{
}

void frame_t::setTexture (vector<byte> const &data_)
{
}
