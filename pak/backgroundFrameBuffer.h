#pragma once

#include "frameBuffer.h"

#include <memory>

class backgroundFrameBuffer_t : public frameBuffer_t
{
public:
    ~backgroundFrameBuffer_t ();
    backgroundFrameBuffer_t ();

    void startFrame () final;

private:
    void _shutdown () final;

    class private_t;
    std::unique_ptr<private_t> m_d;
};
