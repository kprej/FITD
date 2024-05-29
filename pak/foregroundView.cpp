#include "foregroundView.h"
#include "osystem.h"

#include <bgfx/bgfx.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <plog/Log.h>

using namespace std;

namespace
{
}

class foregroundView_t::private_t
{
public:
    ~private_t () {}
    private_t () {}
};

foregroundView_t::~foregroundView_t () = default;

foregroundView_t::foregroundView_t ()
    : view_t (3)
    , m_d (make_unique<private_t> ())
{
}

void foregroundView_t::init ()
{
    bgfx::setViewName (m_viewId, "Foreground");
}

void foregroundView_t::startFrame ()
{
    if (GS ()->screenSizeChanged || !bgfx::isValid (m_frameBuffer))
    {
        PLOGD << "Create Foreground Buffer";
        if (bgfx::isValid (m_frameBuffer))
            bgfx::destroy (m_frameBuffer);

        m_frameBuffer = bgfx::createFrameBuffer (
            GS ()->width, GS ()->height, bgfx::TextureFormat::BGRA8);

        auto const ortho = glm::ortho (0.0f, 320.f, 0.f, 200.0f, -100.1f, 1000.0f);

        bgfx::setViewTransform (m_viewId, NULL, glm::value_ptr (ortho));
        bgfx::setViewFrameBuffer (m_viewId, m_frameBuffer);

        bgfx::setViewRect (m_viewId, 0, 0, GS ()->width, GS ()->height);
    }

    bgfx::setViewClear (m_viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0, 1.0f, 0);
    bgfx::touch (m_viewId);
}
void foregroundView_t::_shutdown ()
{
}
