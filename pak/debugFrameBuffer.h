#pragma once

#include "frameBuffer.h"

#include <memory>

class debugFrameBuffer_t : public frameBuffer_t
{
public:
    ~debugFrameBuffer_t ();
    debugFrameBuffer_t ();

    void startFrame () final;

private:
    void _shutdown () final;

    class private_t;
    std::unique_ptr<private_t> m_d;
};
