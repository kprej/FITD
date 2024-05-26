#include "foregroundView.h"
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

namespace
{
textureVert_t const BACKGROUND_VERT[6] = {{-1.f, -1.f, 0.f, 0.f, 1.f},
                                          {1.f, -1.f, 0.f, 1.f, 1.f},
                                          {1.f, 1.f, 0.f, 1.f, 0.f},

                                          {-1.f, -1.f, 0.f, 0.f, 1.f},
                                          {-1.f, 1.f, 0.f, 0.f, 0.f},
                                          {1.f, 1.f, 0.f, 1.f, 0.f}};
}

class foregroundView_t::private_t
{
public:
    ~private_t () {}
    private_t ()
        : viewId (3)
        , frameBuffer (BGFX_INVALID_HANDLE)
        , foregroundTexture (BGFX_INVALID_HANDLE)
        , foregroundTextureUniform (BGFX_INVALID_HANDLE)
        , foregroundVertexBuffer (BGFX_INVALID_HANDLE)
        , foregroundShader (BGFX_INVALID_HANDLE)
        , isSet (false)
    {
    }

    uint8_t viewId;
    bgfx::FrameBufferHandle frameBuffer;

    bgfx::TextureHandle foregroundTexture;
    bgfx::UniformHandle foregroundTextureUniform;
    bgfx::VertexBufferHandle foregroundVertexBuffer;

    bgfx::ProgramHandle foregroundShader;

    bool isSet;
};

foregroundView_t::~foregroundView_t () = default;

foregroundView_t::foregroundView_t ()
    : m_d (make_unique<private_t> ())
{
}

void foregroundView_t::init ()
{
    m_d->foregroundTextureUniform =
        bgfx::createUniform ("s_foregroundTexture", bgfx::UniformType::Sampler);

    m_d->foregroundTexture =
        bgfx::createTexture2D (320, 200, false, 1, bgfx::TextureFormat::R8U);

    m_d->foregroundVertexBuffer = bgfx::createVertexBuffer (
        bgfx::makeRef (BACKGROUND_VERT, sizeof (textureVert_t) * 6),
        GS ()->handle.textureVertexLayout ());

    m_d->foregroundShader = loadProgram ("background");

    bgfx::setViewName (m_d->viewId, "Foreground");
}

void foregroundView_t::startFrame ()
{
    if (!GS ()->screenSizeChanged)
        return;

    PLOGD << "Create Foreground Buffer";
    if (bgfx::isValid (m_d->frameBuffer))
        bgfx::destroy (m_d->frameBuffer);

    m_d->frameBuffer =
        bgfx::createFrameBuffer (GS ()->width, GS ()->height, bgfx::TextureFormat::BGRA8);

    bgfx::setViewFrameBuffer (m_d->viewId, m_d->frameBuffer);
    bgfx::setViewRect (m_d->viewId, 0, 0, GS ()->width, GS ()->height);
}

void foregroundView_t::update (vector<byte> const &texture_, int offset_)
{
    bgfx::updateTexture2D (
        m_d->foregroundTexture,
        0,
        0,
        0,
        0,
        320,
        200,
        bgfx::copy (texture_.data () + offset_, texture_.size () - offset_));

    m_d->isSet = true;
}

void foregroundView_t::clear ()
{
    m_d->isSet = false;
}

void foregroundView_t::fill (uint8_t color_)
{
    vector<byte> const color (320 * 200, byte (color_));
    bgfx::updateTexture2D (m_d->foregroundTexture,
                           0,
                           0,
                           0,
                           0,
                           320,
                           200,
                           bgfx::copy (color.data (), color.size ()));

    m_d->isSet = true;
}

void foregroundView_t::render ()
{
    bgfx::setViewClear (m_d->viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0, 1.0f, 0);

    bgfx::touch (m_d->viewId);

    if (!m_d->isSet)
        return;

    bgfx::setVertexBuffer (0, m_d->foregroundVertexBuffer);

    bgfx::setState (0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z |
                    BGFX_STATE_BLEND_FUNC (BGFX_STATE_BLEND_SRC_ALPHA,
                                           BGFX_STATE_BLEND_INV_SRC_ALPHA));

    bgfx::setTexture (0, m_d->foregroundTextureUniform, m_d->foregroundTexture);

    GS ()->handle.applyPalette (1);

    bgfx::submit (m_d->viewId, m_d->foregroundShader);
}

void foregroundView_t::shutdown ()
{
    bgfx::destroy (m_d->foregroundShader);
    bgfx::destroy (m_d->foregroundVertexBuffer);
    bgfx::destroy (m_d->foregroundTexture);
    bgfx::destroy (m_d->foregroundTextureUniform);
    bgfx::destroy (m_d->frameBuffer);
}

bgfx::TextureHandle foregroundView_t::texture ()
{
    return bgfx::getTexture (m_d->frameBuffer);
}
