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

class osystem_t;

class bgfxHandle_t
{
public:
    ~bgfxHandle_t ();
    bgfxHandle_t ();

    void init ();
    void startFrame ();
    void endFrame ();

    void shutdown ();

    void createFontTexture (uint16_t height_, uint16_t width_);

    void setPalette (std::vector<std::byte> const &palette_);
    void setBackground (std::vector<std::byte> const &texture_, int offest_ = 0);
    void
    addText (std::vector<std::byte> const &texture_, uint16_t xOffset_, uint8_t width_);

    void fadeInBackground (int msec_ = 500);
    void fadeOutBackground (int msec_ = 500);
    void drawBody (body_t const &body_);

    backgroundState_t backgroundState () const;

    bgfx::VertexLayout const &bodyVertexLayout () const;

protected:
    friend class osystem_t;

    bool windowHidden () const;

private:
    void createLayouts ();
    void createTextureHandles ();
    void createBackgroundVert ();
    void createUniforms ();

    void debug ();

    void drawBackground ();

    void processFadeIn ();
    void processFadeOut ();

    class private_t;
    std::unique_ptr<private_t> m_d;
};
