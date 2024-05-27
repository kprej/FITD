#pragma once

#include "view.h"

#include <bgfx/bgfx.h>

#include <memory>

class bgfxHandle_t;
class bodyView_t : public view_t
{
public:
    ~bodyView_t ();
    bodyView_t ();

protected:
    friend class bgfxHandle_t;
    void init () final;
    void startFrame () final;

private:
    void _shutdown () final;
    void setupFrameBuffer ();

    class private_t;
    std::unique_ptr<private_t> m_d;
};
