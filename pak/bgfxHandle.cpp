#include "bgfxHandle.h"
#include "backgroundView.h"
#include "bgfxShader.h"
#include "bodyView.h"
#include "foregroundView.h"
#include "osystem.h"
#include "vertTypes.h"

#include <bgfx/platform.h>
#include <bx/bx.h>

#include <plog/Helpers/HexDump.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <imgui.h>

#include <plog/Log.h>

using namespace std;

namespace
{
textureVert_t const FULLSCREEN_TEXTURE_VERT[6] = {{0, 0, 0.f, 0.f, 0.f},
                                                  {320.0f, 0.f, 0.f, 1.f, 0.f},
                                                  {320.0f, 200.0f, 0.f, 1.f, 1.f},

                                                  {0.f, 0.f, 0.f, 0.f, 0.f},
                                                  {0.f, 200.f, 0.f, 0.f, 1.f},
                                                  {320.f, 200.f, 0.f, 1.f, 1.f}};

rawBody_t const BOUNDING_VERT[8] = {{-0.5, -0.5, -0.5},
                                    {0.5, -0.5, -0.5},
                                    {0.5, 0.5, -0.5},
                                    {-0.5, 0.5, -0.5},
                                    {-0.5, -0.5, 0.5},
                                    {0.5, -0.5, 0.5},
                                    {0.5, 0.5, 0.5},
                                    {-0.5, 0.5, 0.5}};

uint16_t const BOUNDING_INDEX[16] = {0, 1, 2, 3, 4, 5, 6, 7, 0, 4, 1, 5, 2, 6, 3, 7};
} // namespace

class bgfxHandle_t::private_t
{
public:
    ~private_t () {}

    private_t ()
        : initParam ()
        , fontTexture (BGFX_INVALID_HANDLE)
        , paletteTexture (BGFX_INVALID_HANDLE)
        , combineTextureUniform (BGFX_INVALID_HANDLE)
        , fontTextureUniform (BGFX_INVALID_HANDLE)
        , paletteTextureUniform (BGFX_INVALID_HANDLE)
        , fontColorUniform (BGFX_INVALID_HANDLE)
        , colorUniform (BGFX_INVALID_HANDLE)
        , combineShader (BGFX_INVALID_HANDLE)
        , postShader (BGFX_INVALID_HANDLE)
        , fontShader (BGFX_INVALID_HANDLE)
        , textureShader (BGFX_INVALID_HANDLE)
        , bodyShader (BGFX_INVALID_HANDLE)
        , combineViewId (4)
        , combineFrameBuffer (BGFX_INVALID_HANDLE)
        , renderViewId (0)
        , alpha (255)
    {
    }

    bgfx::Init initParam;

    bgfx::TextureHandle fontTexture;
    bgfx::TextureHandle paletteTexture;

    bgfx::UniformHandle combineTextureUniform;
    bgfx::UniformHandle fontTextureUniform;
    bgfx::UniformHandle paletteTextureUniform;
    bgfx::UniformHandle fontColorUniform;
    bgfx::UniformHandle colorUniform;
    bgfx::UniformHandle polyColorUniform;
    bgfx::UniformHandle fullscreenTextureUniform;

    bgfx::VertexBufferHandle fullscreenTextureVertexBuffer;
    bgfx::VertexBufferHandle boundingBoxVertexBuffer;

    bgfx::IndexBufferHandle boundingBoxIndexBuffer;

    bgfx::VertexLayout textureLayout;
    bgfx::VertexLayout bodyVertexLayout;

    float fadeStep;
    int fadeTimeMSec;
    fadeState_t fadeState;

    // Shader Handles
    bgfx::ProgramHandle combineShader;
    bgfx::ProgramHandle postShader;
    bgfx::ProgramHandle fontShader;
    bgfx::ProgramHandle textureShader;
    bgfx::ProgramHandle bodyShader;

    uint8_t combineViewId;
    bgfx::FrameBufferHandle combineFrameBuffer;

    uint8_t renderViewId;

    backgroundView_t backgroundView;
    bodyView_t bodyView;
    foregroundView_t foregroundView;

    SDL_Window *window;

    float alpha;
};

bgfxHandle_t::~bgfxHandle_t ()
{
}

bgfxHandle_t::bgfxHandle_t ()
    : m_d (make_unique<private_t> ())
{
}

void bgfxHandle_t::init ()
{
    PLOGD << "Create window";
    m_d->window = SDL_CreateWindow (
        "FITD", 1280, 800, SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_HIDDEN);

    if (!m_d->window)
    {
        PLOGF << SDL_GetError ();
        return;
    }

    PLOGD << "Init BGFX";
#if SDL_PLATFORM_WINDOWS
    PLOGD << "Windows Platform";
    m_d->initParam.platformData.ndt = NULL;
    m_d->initParam.platformData.nwh = SDL_GetProperty (
        SDL_GetWindowProperties (m_d->window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
#elif SDL_PLATFORM_LINUX
    PLOGD << "Linux Platform";
    if (SDL_strcmp (SDL_GetCurrentVideoDriver (), "x11") == 0)
    {
        m_d->initParam.platformData.ndt =
            SDL_GetProperty (SDL_GetWindowProperties (m_d->window),
                             SDL_PROP_WINDOW_X11_DISPLAY_POINTER,
                             NULL);
        m_d->initParam.platformData.nwh = (void *)(uintptr_t)(SDL_GetNumberProperty (
            SDL_GetWindowProperties (m_d->window), SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0));
    }
    else if (SDL_strcmp (SDL_GetCurrentVideoDriver (), "wayland") == 0)
    {
        m_d->initParam.platformData.ndt =
            SDL_GetProperty (SDL_GetWindowProperties (m_d->window),
                             SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER,
                             NULL);
        m_d->initParam.platformData.nwh =
            SDL_GetProperty (SDL_GetWindowProperties (m_d->window),
                             SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER,
                             NULL);
    }
#endif

    m_d->initParam.type = bgfx::RendererType::Vulkan;
    m_d->initParam.resolution.width = 320;
    m_d->initParam.resolution.height = 200;
    m_d->initParam.resolution.reset = BGFX_RESET_VSYNC;

    if (!bgfx::init (m_d->initParam))
    {
        PLOGF << "Failed to init bgfx";
        return;
    }

    GS ()->debug.init (m_d->window);
    GS ()->debug.draw.connect<&bgfxHandle_t::debug> (this);

    createLayouts ();
    createTextureHandles ();
    createBuffers ();
    createUniforms ();

    initViews ();

    PLOGD << "Load shaders";
    m_d->combineShader = loadProgram ("combine");
    m_d->textureShader = loadProgram ("texture");
    m_d->bodyShader = loadProgram ("body");
    m_d->postShader = loadProgram ("post");
    m_d->fontShader = loadProgram ("font");
}

void bgfxHandle_t::startFrame ()
{
    if (SDL_GetWindowFlags (m_d->window) & SDL_WINDOW_HIDDEN)
    {
        SDL_SetWindowTitle (m_d->window, toString (GS ()->gameId).c_str ());
    }

    SDL_ShowWindow (m_d->window);
    GS ()->previousScreenSize[0] = GS ()->width;
    GS ()->previousScreenSize[1] = GS ()->height;

    SDL_GetWindowSize (m_d->window, &GS ()->width, &GS ()->height);

    if ((GS ()->previousScreenSize[0] != GS ()->width) ||
        (GS ()->previousScreenSize[1] != GS ()->height))
    {
        GS ()->screenSizeChanged = true;
        bgfx::reset (GS ()->width, GS ()->height);

        m_d->combineFrameBuffer = bgfx::createFrameBuffer (bgfx::BackbufferRatio::Equal,
                                                           bgfx::TextureFormat::BGRA8);

        bgfx::setViewFrameBuffer (m_d->combineViewId, m_d->combineFrameBuffer);
    }
    else
    {
        GS ()->screenSizeChanged = false;
    }

    bgfx::setViewRect (m_d->combineViewId, 0, 0, GS ()->width, GS ()->height);
    bgfx::setViewRect (m_d->renderViewId, 0, 0, GS ()->width, GS ()->height);

    bgfx::setViewFrameBuffer (m_d->renderViewId,
                              BGFX_INVALID_HANDLE); // bind the backbuffer

    bgfx::setViewClear (
        m_d->combineViewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 255, 1.0f, 0);

    bgfx::setViewClear (
        m_d->renderViewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 255, 1.0f, 0);

    bgfx::touch (m_d->combineViewId);
    bgfx::touch (m_d->renderViewId);

    m_d->backgroundView.startFrame ();
    m_d->bodyView.startFrame ();
    m_d->foregroundView.startFrame ();

    if (GS ()->debugMenuDisplayed)
        GS ()->debug.startFrame ();
}

void bgfxHandle_t::endFrame ()
{
    combine (m_d->backgroundView.texture ());
    combine (m_d->bodyView.texture ());
    combine (m_d->foregroundView.texture ());

    if (GS ()->debugMenuDisplayed)
    {
        GS ()->debug.endFrame ();
    }

    processFade ();

    bgfx::setVertexBuffer (0, m_d->fullscreenTextureVertexBuffer);
    bgfx::setState (0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
                    BGFX_STATE_BLEND_FUNC (BGFX_STATE_BLEND_SRC_ALPHA,
                                           BGFX_STATE_BLEND_INV_SRC_ALPHA));
    bgfx::setTexture (
        0, m_d->combineTextureUniform, bgfx::getTexture (m_d->combineFrameBuffer));

    float color[4] = {0, 0, 0, m_d->alpha};
    bgfx::setUniform (m_d->colorUniform, color);
    bgfx::submit (m_d->renderViewId, m_d->postShader);

    bgfx::frame ();
}

void bgfxHandle_t::setPalette (vector<byte> const &palette_)
{
    bgfx::updateTexture2D (
        m_d->paletteTexture, 0, 0, 0, 0, 3, 256, bgfx::copy (palette_.data (), 256 * 3));
}

void bgfxHandle_t::addFontChar (vector<byte> const &texture_,
                                uint16_t xOffset_,
                                uint8_t width_)
{
    bgfx::updateTexture2D (m_d->fontTexture,
                           0,
                           0,
                           xOffset_,
                           0,
                           width_,
                           16,
                           bgfx::copy (texture_.data (), texture_.size ()));
}

void bgfxHandle_t::drawFullscreenBackground (texture_t const &texture_)
{
    drawFullscreen (texture_, true);
}

void bgfxHandle_t::drawToBackground (bgfx::TransientVertexBuffer const &buffer_,
                                     texture_t const &texture_)
{
    drawToScreen (buffer_, texture_, true);
}

void bgfxHandle_t::drawFullscreenForeground (texture_t const &texture_)
{
    drawFullscreen (texture_, false);
}

void bgfxHandle_t::drawToForeground (bgfx::TransientVertexBuffer const &buffer_,
                                     texture_t const &texture_)
{
    drawToScreen (buffer_, texture_, false);
}

void bgfxHandle_t::drawBody (body_t const &body_)
{
    for (auto const &p : body_.primitives ())
    {
        bgfx::setTransform (glm::value_ptr (body_.transform ()));

        bgfx::setVertexBuffer (0, body_.vertexBuffer ());
        bgfx::setIndexBuffer (body_.indexBuffer (), p.start, p.size);

        bgfx::setTexture (1,
                          m_d->paletteTextureUniform,
                          GS ()->palettes.at (body_.palette ()).handle ());

        bgfx::setUniform (m_d->polyColorUniform, &p.color);

        unsigned long long state = 0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
                                   BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS |
                                   BGFX_STATE_CULL_CW;

        if (p.size == 2)
            state |= BGFX_STATE_PT_LINES;

        bgfx::setState (state);
        bgfx::submit (m_d->bodyView.id (), m_d->bodyShader);
    }

    /*
    GS ()->handle.applyPalette (1);
    bgfx::setTransform (glm::value_ptr (body_.transform () * body_.boundingBox ()));
    bgfx::setVertexBuffer (0, m_d->boundingBoxVertexBuffer);
    bgfx::setIndexBuffer (m_d->boundingBoxIndexBuffer);
    unsigned long long state = 0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
                               BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS |
                               BGFX_STATE_CULL_CW | BGFX_STATE_MSAA;

    state |= BGFX_STATE_PT_LINES;

    bgfx::setState (state);
    bgfx::submit (m_d->bodyView.id (), m_d->bodyShader);
    */
}

void bgfxHandle_t::fadeIn (int msec_)
{
    if (m_d->fadeState == fadeState_t::FADING_IN ||
        m_d->fadeState == fadeState_t::VISIBLE)
        return;

    m_d->fadeState = fadeState_t::FADING_IN;
    m_d->fadeStep = 0;
    m_d->fadeTimeMSec = msec_;
    m_d->alpha = 0;
}

void bgfxHandle_t::fadeOut (int msec_)
{
    if (m_d->fadeState == fadeState_t::FADING_OUT ||
        m_d->fadeState == fadeState_t::INVISIBLE)
        return;

    m_d->fadeState = fadeState_t::FADING_OUT;
    m_d->fadeStep = msec_;
    m_d->fadeTimeMSec = msec_;
    m_d->alpha = 1;
}

fadeState_t bgfxHandle_t::fadeState () const
{
    return m_d->fadeState;
}

void bgfxHandle_t::renderText (bgfx::TransientVertexBuffer const &buffer_)
{
    bgfx::setVertexBuffer (0, &buffer_);

    bgfx::setTexture (0, m_d->fontTextureUniform, m_d->fontTexture);

    float color[4] = {255, 23, 230, 255};
    bgfx::setUniform (m_d->fontColorUniform, color);

    bgfx::setState (0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z |
                    BGFX_STATE_BLEND_FUNC (BGFX_STATE_BLEND_SRC_ALPHA,
                                           BGFX_STATE_BLEND_INV_SRC_ALPHA));

    bgfx::submit (m_d->foregroundView.id (), m_d->fontShader);
}

bgfx::VertexLayout const &bgfxHandle_t::bodyVertexLayout () const
{
    return m_d->bodyVertexLayout;
}

bgfx::VertexLayout const &bgfxHandle_t::textureVertexLayout () const
{
    return m_d->textureLayout;
}

void bgfxHandle_t::shutdown ()
{
    PLOGD << "Destroy shaders";

    m_d->backgroundView.shutdown ();
    m_d->bodyView.shutdown ();
    m_d->foregroundView.shutdown ();
    GS ()->debug.shutdown ();

    for (auto const &tex : GS ()->textures)
        bgfx::destroy (tex.second.handle ());

    for (auto const &tex : GS ()->palettes)
        bgfx::destroy (tex.second.handle ());

    bgfx::destroy (m_d->bodyShader);
    bgfx::destroy (m_d->postShader);
    bgfx::destroy (m_d->textureShader);
    bgfx::destroy (m_d->fontShader);
    bgfx::destroy (m_d->combineShader);

    bgfx::destroy (m_d->combineFrameBuffer);

    bgfx::destroy (m_d->fullscreenTextureVertexBuffer);
    bgfx::destroy (m_d->boundingBoxVertexBuffer);
    bgfx::destroy (m_d->boundingBoxIndexBuffer);

    bgfx::destroy (m_d->fontTexture);
    bgfx::destroy (m_d->paletteTexture);

    bgfx::destroy (m_d->colorUniform);
    bgfx::destroy (m_d->polyColorUniform);
    bgfx::destroy (m_d->fontTextureUniform);
    bgfx::destroy (m_d->fullscreenTextureUniform);
    bgfx::destroy (m_d->paletteTextureUniform);
    bgfx::destroy (m_d->fontColorUniform);
    bgfx::destroy (m_d->combineTextureUniform);

    PLOGD << "Shutdown BGFX";
    bgfx::shutdown ();

    SDL_DestroyWindow (m_d->window);
}

bool bgfxHandle_t::windowHidden () const
{
    return (SDL_GetWindowFlags (m_d->window) & SDL_WINDOW_HIDDEN);
}

void bgfxHandle_t::createFontTexture (uint16_t height_, uint16_t width_)
{
    m_d->fontTexture =
        bgfx::createTexture2D (width_, height_, false, 1, bgfx::TextureFormat::R8U);
}

void bgfxHandle_t::createLayouts ()
{
    m_d->textureLayout.begin ()
        .add (bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add (bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end ();

    m_d->bodyVertexLayout.begin ()
        .add (bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .end ();
}

void bgfxHandle_t::createTextureHandles ()
{
    PLOGD << "Create Textures Handles";
    m_d->paletteTexture =
        bgfx::createTexture2D (3, 256, false, 1, bgfx::TextureFormat::R8U);
}

void bgfxHandle_t::createBuffers ()
{
    m_d->fullscreenTextureVertexBuffer = bgfx::createVertexBuffer (
        bgfx::makeRef (FULLSCREEN_TEXTURE_VERT, sizeof (textureVert_t) * 6),
        m_d->textureLayout);

    m_d->boundingBoxVertexBuffer = bgfx::createVertexBuffer (
        bgfx::makeRef (BOUNDING_VERT, sizeof (rawBody_t) * 8), m_d->bodyVertexLayout);

    m_d->boundingBoxIndexBuffer =
        bgfx::createIndexBuffer (bgfx::makeRef (BOUNDING_INDEX, sizeof (uint16_t) * 16));
}

void bgfxHandle_t::createUniforms ()
{
    m_d->combineTextureUniform =
        bgfx::createUniform ("s_combineTexture", bgfx::UniformType::Sampler);

    m_d->fontTextureUniform =
        bgfx::createUniform ("s_fontTexture", bgfx::UniformType::Sampler);

    m_d->paletteTextureUniform =
        bgfx::createUniform ("s_paletteTexture", bgfx::UniformType::Sampler);

    m_d->fullscreenTextureUniform =
        bgfx::createUniform ("s_backgroundTexture", bgfx::UniformType::Sampler);

    m_d->fontColorUniform = bgfx::createUniform ("s_fontColor", bgfx::UniformType::Vec4);
    m_d->colorUniform = bgfx::createUniform ("s_color", bgfx::UniformType::Vec4);
    m_d->polyColorUniform = bgfx::createUniform ("s_polyColor", bgfx::UniformType::Vec4);
}

void bgfxHandle_t::initViews ()
{
    m_d->backgroundView.init ();
    m_d->bodyView.init ();
    m_d->foregroundView.init ();
}

void bgfxHandle_t::debug ()
{
    if (ImGui::Begin ("BGFX"))
    {
        switch (m_d->fadeState)
        {
        case fadeState_t::INVISIBLE:
            ImGui::Text ("FadeState: Invisible: %f", m_d->alpha);
            break;
        case fadeState_t::VISIBLE:
            ImGui::Text ("FadeState: Visible: %f", m_d->alpha);
            break;
        case fadeState_t::FADING_IN:
            ImGui::Text ("FadeState: Fading In: %f", m_d->alpha);
            break;
        case fadeState_t::FADING_OUT:
            ImGui::Text ("FadeState: Fading Out: %f", m_d->alpha);
            break;
        }
    }
    ImGui::End ();
}

void bgfxHandle_t::processFade ()
{
    if (m_d->fadeState == fadeState_t::FADING_IN)
    {
        m_d->fadeStep += GS ()->delta;
        m_d->alpha =
            glm::clamp (lerp (0.f, 1.f, m_d->fadeStep / m_d->fadeTimeMSec), 0.f, 1.f);

        if (m_d->alpha < 1.f)
            m_d->fadeState = fadeState_t::FADING_IN;

        else if (m_d->alpha == 1)
            m_d->fadeState = fadeState_t::VISIBLE;

        return;
    }
    if (m_d->fadeState == fadeState_t::FADING_OUT)
    {
        m_d->fadeStep -= GS ()->delta;
        m_d->alpha =
            glm::clamp (lerp (0.f, 1.f, m_d->fadeStep / m_d->fadeTimeMSec), 0.f, 1.f);
        if (m_d->alpha > 0.f)
            m_d->fadeState = fadeState_t::FADING_OUT;

        else if (m_d->alpha == 0)
            m_d->fadeState = fadeState_t::INVISIBLE;

        return;
    }
}

void bgfxHandle_t::drawFullscreen (texture_t const &texture_, bool background_)
{
    bgfx::setVertexBuffer (0, m_d->fullscreenTextureVertexBuffer);

    bgfx::setState (0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z |
                    BGFX_STATE_BLEND_FUNC (BGFX_STATE_BLEND_SRC_ALPHA,
                                           BGFX_STATE_BLEND_INV_SRC_ALPHA));

    bgfx::setTexture (0, m_d->fullscreenTextureUniform, texture_.handle ());
    bgfx::setTexture (1,
                      m_d->paletteTextureUniform,
                      GS ()->palettes.at (texture_.palette ()).handle ());

    bgfx::submit (background_ ? m_d->backgroundView.id () : m_d->foregroundView.id (),
                  m_d->textureShader);
}

void bgfxHandle_t::drawToScreen (bgfx::TransientVertexBuffer const &buffer_,
                                 texture_t const &texture_,
                                 bool background_)
{
    bgfx::setVertexBuffer (0, &buffer_);

    bgfx::setState (0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z |
                    BGFX_STATE_BLEND_FUNC (BGFX_STATE_BLEND_SRC_ALPHA,
                                           BGFX_STATE_BLEND_INV_SRC_ALPHA));

    bgfx::setTexture (0, m_d->fullscreenTextureUniform, texture_.handle ());
    bgfx::setTexture (1,
                      m_d->paletteTextureUniform,
                      GS ()->palettes.at (texture_.palette ()).handle ());

    bgfx::submit (background_ ? m_d->backgroundView.id () : m_d->foregroundView.id (),
                  m_d->textureShader);
}

void bgfxHandle_t::combine (bgfx::TextureHandle const &handle_)
{
    bgfx::setVertexBuffer (0, m_d->fullscreenTextureVertexBuffer);

    bgfx::setState (0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
                    BGFX_STATE_BLEND_FUNC (BGFX_STATE_BLEND_SRC_ALPHA,
                                           BGFX_STATE_BLEND_INV_SRC_ALPHA));

    bgfx::setTexture (0, m_d->combineTextureUniform, handle_);

    bgfx::submit (m_d->combineViewId, m_d->combineShader);
}
