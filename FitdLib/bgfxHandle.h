#pragma once

#include <bgfx/bgfx.h>

#include <SDL3/SDL.h>

#include <memory>

class bgfxHandle_t
{
public:
    ~bgfxHandle_t ();
    bgfxHandle_t ();

    void init ();
    void startFrame ();

private:
    void startDebugFrame ();
    void drawBackground ();

    class private_t;

    std::unique_ptr<private_t> m_d;
};
