#pragma once

#include "body.h"
#include "texture.h"

#include <bgfx/bgfx.h>

#include <nano_signal_slot.hpp>

#include <SDL3/SDL.h>

#include <memory>
#include <vector>

class osystem_t;

enum class fadeState_t
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

    void createFontTexture (uint16_t height_, uint16_t width_);

    void setPalette (std::vector<std::byte> const &palette_);

    void addFontChar (std::vector<std::byte> const &texture_,
                      uint16_t xOffset_,
                      uint8_t width_);

    void drawFullscreenBackground (texture_t const &texture_);
    void drawFullscreenForeground (texture_t const &texture_);

    void drawBody (body_t const &body_);

    void drawToBackground (bgfx::TransientVertexBuffer const &buffer_,
                           texture_t const &texture_);
    void drawToForeground (bgfx::TransientVertexBuffer const &buffer_,
                           texture_t const &texture_);

    void fadeIn (int msec_);
    void fadeOut (int msec_);

    fadeState_t fadeState () const;

    void renderText (bgfx::TransientVertexBuffer const &buffer_);

    bgfx::VertexLayout const &bodyVertexLayout () const;
    bgfx::VertexLayout const &textureVertexLayout () const;

    /// \name Signals
    /// @{
    Nano::Signal<void ()> cleanup;
    /// @}

protected:
    friend class osystem_t;

    void init ();
    void startFrame ();
    void endFrame ();

    void shutdown ();
    bool windowHidden () const;

private:
    void createLayouts ();
    void createTextureHandles ();
    void createBuffers ();
    void createUniforms ();

    void initViews ();

    void debug ();

    void processFade ();

    void drawFullscreen (texture_t const &texture_, bool background_);
    void drawToScreen (bgfx::TransientVertexBuffer const &buffer_,
                       texture_t const &texture_,
                       bool background_);

    void combine (bgfx::TextureHandle const &handle_);

    class private_t;
    std::unique_ptr<private_t> m_d;
};
