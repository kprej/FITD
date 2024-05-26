#include "backgroundView.h"
#include "bgfxHandle.h"
#include "bgfxShader.h"
#include "osystem.h"
#include "vertTypes.h"

#include <bgfx/bgfx.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <plog/Log.h>

using namespace std;

class backgroundView_t::private_t
{
public:
    ~private_t () {}
    private_t ()
        : viewId (1)
        , frameBuffer (BGFX_INVALID_HANDLE)
        , backgroundTexture (BGFX_INVALID_HANDLE)
        , backgroundTextureUniform (BGFX_INVALID_HANDLE)
        , backgroundShader (BGFX_INVALID_HANDLE)
    {
    }

    uint8_t viewId;
    bgfx::FrameBufferHandle frameBuffer;

    bgfx::TextureHandle backgroundTexture;
    bgfx::UniformHandle backgroundTextureUniform;

    bgfx::ProgramHandle backgroundShader;
};

backgroundView_t::~backgroundView_t () = default;

backgroundView_t::backgroundView_t ()
    : m_d (make_unique<private_t> ())
{
}

void backgroundView_t::update (vector<byte> const &texture_, int offset_)
{
    bgfx::updateTexture2D (
        m_d->backgroundTexture,
        0,
        0,
        0,
        0,
        320,
        200,
        bgfx::copy (texture_.data () + offset_, texture_.size () - offset_));
}

void backgroundView_t::blackout (glm::tvec2<unsigned> const &pointA_,
                                 glm::tvec2<unsigned> const &pointB_)
{
    vector<byte> const blackout ((pointB_.x - pointA_.x) * (pointB_.y - pointA_.y),
                                 byte (0));
    bgfx::updateTexture2D (m_d->backgroundTexture,
                           0,
                           0,
                           pointA_.x,
                           pointA_.y,
                           pointB_.x,
                           pointB_.y,
                           bgfx::copy (blackout.data (), blackout.size ()));
}

void backgroundView_t::init ()
{
    m_d->backgroundTextureUniform =
        bgfx::createUniform ("s_backgroundTexture", bgfx::UniformType::Sampler);

    m_d->backgroundTexture =
        bgfx::createTexture2D (320, 200, false, 1, bgfx::TextureFormat::R8U);

    m_d->backgroundShader = loadProgram ("background");

    bgfx::setViewName (m_d->viewId, "Background");
}

void backgroundView_t::startFrame ()
{
    if (!GS ()->screenSizeChanged)
        return;

    PLOGD << "Create Background Buffer";
    if (bgfx::isValid (m_d->frameBuffer))
        bgfx::destroy (m_d->frameBuffer);

    m_d->frameBuffer =
        bgfx::createFrameBuffer (GS ()->width, GS ()->height, bgfx::TextureFormat::BGRA8);

    bgfx::setViewFrameBuffer (m_d->viewId, m_d->frameBuffer);
    bgfx::setViewRect (m_d->viewId, 0, 0, GS ()->width, GS ()->height);
}

void backgroundView_t::render ()
{
    bgfx::setViewClear (m_d->viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 255, 1.0f, 0);

    bgfx::touch (m_d->viewId);

    bgfx::setVertexBuffer (0, GS ()->handle.textureVertexBuffer ());

    bgfx::setState (0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z |
                    BGFX_STATE_BLEND_FUNC (BGFX_STATE_BLEND_SRC_ALPHA,
                                           BGFX_STATE_BLEND_INV_SRC_ALPHA));

    bgfx::setTexture (0, m_d->backgroundTextureUniform, m_d->backgroundTexture);

    GS ()->handle.applyPalette (1);

    bgfx::submit (m_d->viewId, m_d->backgroundShader);
}

void backgroundView_t::shutdown ()
{
    bgfx::destroy (m_d->backgroundShader);
    bgfx::destroy (m_d->backgroundTexture);
    bgfx::destroy (m_d->backgroundTextureUniform);
    bgfx::destroy (m_d->frameBuffer);
}

bgfx::TextureHandle backgroundView_t::texture ()
{
    return bgfx::getTexture (m_d->frameBuffer);
}
