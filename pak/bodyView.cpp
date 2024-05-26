#include "bodyView.h"
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

class bodyView_t::private_t
{
public:
    ~private_t () {}
    private_t ()
        : viewId (2)
        , frameBuffer (BGFX_INVALID_HANDLE)
        , polyColorUniform (BGFX_INVALID_HANDLE)
        , flatShader (BGFX_INVALID_HANDLE)
        , fieldModelInspectorTexture (BGFX_INVALID_HANDLE)
        , fieldModelInspectorDepth (BGFX_INVALID_HANDLE)
        , boundingBoxVertexBuffer (BGFX_INVALID_HANDLE)
        , boundingBoxIndexBuffer (BGFX_INVALID_HANDLE)
    {
    }

    uint8_t viewId;
    bgfx::FrameBufferHandle frameBuffer;

    bgfx::UniformHandle polyColorUniform;

    bgfx::ProgramHandle flatShader;
    bgfx::TextureHandle fieldModelInspectorTexture;
    bgfx::TextureHandle fieldModelInspectorDepth;

    bgfx::VertexBufferHandle boundingBoxVertexBuffer;
    bgfx::IndexBufferHandle boundingBoxIndexBuffer;
};

bodyView_t::~bodyView_t () = default;

bodyView_t::bodyView_t ()
    : m_d (make_unique<private_t> ())
{
}

void bodyView_t::drawBody (body_t const &body_)
{
    for (auto const &p : body_.primitives ())
    {
        bgfx::setTransform (glm::value_ptr (body_.transform ()));

        bgfx::setVertexBuffer (0, body_.vertexBuffer ());
        bgfx::setIndexBuffer (body_.indexBuffer (), p.start, p.size);

        GS ()->handle.applyPalette (1);

        bgfx::setUniform (m_d->polyColorUniform, &p.color);

        unsigned long long state = 0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
                                   BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS |
                                   BGFX_STATE_CULL_CW;

        if (p.size == 2)
            state |= BGFX_STATE_PT_LINES;

        bgfx::setState (state);
        bgfx::submit (m_d->viewId, m_d->flatShader);
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
    bgfx::submit (m_d->viewId, m_d->flatShader);
    */
}

void bodyView_t::init ()
{
    m_d->polyColorUniform = bgfx::createUniform ("s_polyColor", bgfx::UniformType::Vec4);

    m_d->flatShader = loadProgram ("flat");

    m_d->boundingBoxVertexBuffer =
        bgfx::createVertexBuffer (bgfx::makeRef (BOUNDING_VERT, sizeof (rawBody_t) * 8),
                                  GS ()->handle.bodyVertexLayout ());

    m_d->boundingBoxIndexBuffer =
        bgfx::createIndexBuffer (bgfx::makeRef (BOUNDING_INDEX, sizeof (uint16_t) * 16));

    bgfx::setViewName (m_d->viewId, "Bodies");
}

void bodyView_t::startFrame ()
{

    bgfx::setViewRect (m_d->viewId,
                       GS ()->camera.viewPos ().x,
                       GS ()->camera.viewPos ().y,
                       GS ()->camera.viewSize ().x,
                       GS ()->camera.viewSize ().y);

    bgfx::setViewClear (m_d->viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0, 1.0f, 0);

    bgfx::setViewTransform (m_d->viewId,
                            glm::value_ptr (GS ()->camera.view ()),
                            glm::value_ptr (GS ()->camera.projection ()));

    bgfx::touch (m_d->viewId);

    if (!GS ()->screenSizeChanged || bgfx::isValid (m_d->frameBuffer))
        return;

    PLOGD << "Create Body Buffer";
    if (bgfx::isValid (m_d->frameBuffer))
        bgfx::destroy (m_d->frameBuffer);

    const uint64_t tsFlags = 0
                             //| BGFX_SAMPLER_MIN_POINT
                             //| BGFX_SAMPLER_MAG_POINT
                             //| BGFX_SAMPLER_MIP_POINT
                             | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;

    m_d->fieldModelInspectorTexture = bgfx::createTexture2D (GS ()->width,
                                                             GS ()->height,
                                                             false,
                                                             0,
                                                             bgfx::TextureFormat::BGRA8,
                                                             BGFX_TEXTURE_RT | tsFlags);

    m_d->fieldModelInspectorDepth = bgfx::createTexture2D (GS ()->width,
                                                           GS ()->height,
                                                           false,
                                                           0,
                                                           bgfx::TextureFormat::D24S8,
                                                           BGFX_TEXTURE_RT | tsFlags);
    array<bgfx::Attachment, 2> attachements;
    attachements[0].init (m_d->fieldModelInspectorTexture);
    attachements[1].init (m_d->fieldModelInspectorDepth);

    m_d->frameBuffer = bgfx::createFrameBuffer (2, &attachements[0], true);

    bgfx::setViewFrameBuffer (m_d->viewId, m_d->frameBuffer);
}

void bodyView_t::shutdown ()
{
    bgfx::destroy (m_d->flatShader);
    bgfx::destroy (m_d->frameBuffer);
    bgfx::destroy (m_d->polyColorUniform);
    bgfx::destroy (m_d->boundingBoxVertexBuffer);
    bgfx::destroy (m_d->boundingBoxIndexBuffer);
}

bgfx::TextureHandle bodyView_t::texture ()
{
    return bgfx::getTexture (m_d->frameBuffer);
}
