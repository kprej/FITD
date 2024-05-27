#include "view.h"

view_t::~view_t () = default;

view_t::view_t (uint8_t id_)
    : m_viewId (id_)
    , m_frameBuffer (BGFX_INVALID_HANDLE)
{
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
