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

class bodyView_t::private_t
{
public:
    ~private_t () {}
    private_t ()
        : fieldModelInspectorTexture (BGFX_INVALID_HANDLE)
        , fieldModelInspectorDepth (BGFX_INVALID_HANDLE)
    {
    }

    bgfx::TextureHandle fieldModelInspectorTexture;
    bgfx::TextureHandle fieldModelInspectorDepth;
};

bodyView_t::~bodyView_t () = default;

bodyView_t::bodyView_t ()
    : view_t (2)
    , m_d (make_unique<private_t> ())
{
}

void bodyView_t::init ()
{
    bgfx::setViewName (m_viewId, "Bodies");
}

void bodyView_t::startFrame ()
{
    if (GS ()->screenSizeChanged || !bgfx::isValid (m_frameBuffer))
        setupFrameBuffer ();

    bgfx::setViewRect (m_viewId,
                       GS ()->camera.viewPos ().x,
                       GS ()->camera.viewPos ().y,
                       GS ()->camera.viewSize ().x,
                       GS ()->camera.viewSize ().y);

    bgfx::setViewClear (m_viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0, 1.0f, 0);

    bgfx::setViewTransform (m_viewId,
                            glm::value_ptr (GS ()->camera.view ()),
                            glm::value_ptr (GS ()->camera.projection ()));

    bgfx::touch (m_viewId);
}

void bodyView_t::_shutdown ()
{
}

void bodyView_t::setupFrameBuffer ()
{
    PLOGD << "Create Body Buffer";
    if (bgfx::isValid (m_frameBuffer))
        bgfx::destroy (m_frameBuffer);

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

    m_frameBuffer = bgfx::createFrameBuffer (2, &attachements[0], true);

    bgfx::setViewFrameBuffer (m_viewId, m_frameBuffer);
}
