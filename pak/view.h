#pragma once

#include <bgfx/bgfx.h>

class bgfxHandle_t;
class view_t
{
public:
    virtual ~view_t () = 0;

    uint8_t id () const;
    bgfx::TextureHandle texture () { return bgfx::getTexture (m_frameBuffer); };

protected:
    view_t (uint8_t id_);

    void shutdown ();

    virtual void init () = 0;
    virtual void startFrame () = 0;

    uint8_t m_viewId;
    bgfx::FrameBufferHandle m_frameBuffer;

private:
    virtual void _shutdown () = 0;
};
