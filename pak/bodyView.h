#pragma once

#include "body.h"

#include <bgfx/bgfx.h>

#include <memory>

class bgfxHandle_t;
class bodyView_t
{
public:
    ~bodyView_t ();
    bodyView_t ();

    void drawBody (body_t const &body_);

protected:
    friend class bgfxHandle_t;
    void init ();
    void startFrame ();
    void render ();
    void shutdown ();
    bgfx::TextureHandle texture ();

private:
    class private_t;
    std::unique_ptr<private_t> m_d;
};
