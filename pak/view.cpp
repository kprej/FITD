#include "view.h"
#include "osystem.h"

view_t::~view_t () = default;

view_t::view_t (uint8_t id_)
    : m_viewId (id_)
    , m_frameBuffer (BGFX_INVALID_HANDLE)
{
    GS ()->handle.cleanup.connect<&view_t::shutdown> (this);
}

uint8_t view_t::id () const
{
    return m_viewId;
}

void view_t::shutdown ()
{
    if (bgfx::isValid (m_frameBuffer))
        bgfx::destroy (m_frameBuffer);

    _shutdown ();
}
