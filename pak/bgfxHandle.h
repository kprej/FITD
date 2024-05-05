#pragma once

#include <bgfx/bgfx.h>

#include <SDL3/SDL.h>

#include <memory>
#include <vector>

class bgfxHandle_t
{
public:
    ~bgfxHandle_t ();
    bgfxHandle_t ();

    void init ();
    void startFrame ();

    void shutdown ();

    void setPalette (std::vector<std::byte> const &palette_);
    void setBackground (std::vector<std::byte> const &texture_, int offest_);

private:
    void startDebugFrame ();
    void drawBackground ();

    class private_t;
    std::unique_ptr<private_t> m_d;
};
