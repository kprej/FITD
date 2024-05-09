#pragma once

#include "body.h"

#include <bgfx/bgfx.h>

#include <SDL3/SDL.h>

#include <memory>
#include <vector>

enum class backgroundState_t
{
    VISIBLE,
    INVISIBLE,
    FADING_IN,
    FADING_OUT,
};

class bgfxHandle_t
{
public:
    ~bgfxHandle_t ();
    bgfxHandle_t ();

    void init ();
    void startFrame ();
    void endFrame ();

    void shutdown ();

    void setPalette (std::vector<std::byte> const &palette_);
    void setBackground (std::vector<std::byte> const &texture_, int offest_ = 0);

    void fadeInBackground (float step_);
    void fadeOutBackground (float step_);
    void drawBody (body_t const &body_);

    backgroundState_t backgroundState () const;

    bgfx::VertexLayout const &bodyVertexLayout () const;

private:
    void startDebugFrame ();
    void drawBackground ();

    class private_t;
    std::unique_ptr<private_t> m_d;
};
