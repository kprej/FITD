#pragma once

#include <bgfx/bgfx.h>

#include <memory>
#include <vector>

class foregroundView_t
{
public:
    ~foregroundView_t ();
    foregroundView_t ();

    void init ();

    void startFrame ();

    void update (std::vector<std::byte> const &texture_, int offset_);
    void clear ();
    void fill (uint8_t color_);

    void render ();

    void shutdown ();

    bgfx::TextureHandle texture ();

private:
    class private_t;
    std::unique_ptr<private_t> m_d;
};
