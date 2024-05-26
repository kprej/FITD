#pragma once

#include <bgfx/bgfx.h>

class frameBuffer_t
{
public:
    virtual ~frameBuffer_t () = 0;

    void shutdown ();
    virtual void startFrame () = 0;

    bgfx::TextureHandle endFrame ();

    uint8_t viewId () const;

protected:
    frameBuffer_t (uint8_t id_);

    void setFrameBuffer (bgfx::FrameBufferHandle handle_);

    virtual void _shutdown () = 0;

private:
    uint8_t m_viewId;
    bgfx::FrameBufferHandle m_frameBuffer;
};
