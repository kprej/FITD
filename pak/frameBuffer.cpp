#include "frameBuffer.h"

using namespace std;

frameBuffer_t::~frameBuffer_t () = default;

frameBuffer_t::frameBuffer_t (uint8_t id_)
    : m_viewId (id_)
    , m_frameBuffer (BGFX_INVALID_HANDLE)
{
}

void frameBuffer_t::shutdown ()
{
    bgfx::destroy (m_frameBuffer);

    _shutdown ();
}

bgfx::TextureHandle frameBuffer_t::endFrame ()
{
    return bgfx::getTexture (m_frameBuffer);
}

uint8_t frameBuffer_t::viewId () const
{
    return m_viewId;
}

void frameBuffer_t::setFrameBuffer (bgfx::FrameBufferHandle handle_)
{
    if (bgfx::isValid (m_frameBuffer))
        bgfx::destroy (m_frameBuffer);

    m_frameBuffer = handle_;

    bgfx::setViewFrameBuffer (m_viewId, m_frameBuffer);
}
