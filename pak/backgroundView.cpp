#include "backgroundView.h"
#include "osystem.h"

#include <bgfx/bgfx.h>

#include <plog/Log.h>

using namespace std;

class backgroundView_t::private_t
{
public:
    ~private_t () {}
    private_t () {}
};

backgroundView_t::~backgroundView_t () = default;

backgroundView_t::backgroundView_t ()
    : view_t (1)
    , m_d (make_unique<private_t> ())
{
}

void backgroundView_t::init ()
{
    bgfx::setViewName (m_viewId, "Background");
}

void backgroundView_t::startFrame ()
{
    if (GS ()->screenSizeChanged || !bgfx::isValid (m_frameBuffer))
    {
        PLOGD << "Create Background Buffer";
        if (bgfx::isValid (m_frameBuffer))
            bgfx::destroy (m_frameBuffer);

        m_frameBuffer = bgfx::createFrameBuffer (
            GS ()->width, GS ()->height, bgfx::TextureFormat::BGRA8);

        bgfx::setViewFrameBuffer (m_viewId, m_frameBuffer);
        bgfx::setViewRect (m_viewId, 0, 0, GS ()->width, GS ()->height);
    }

    bgfx::setViewClear (m_viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0, 1.0f, 0);
    bgfx::touch (m_viewId);
}

void backgroundView_t::_shutdown ()
{
}
