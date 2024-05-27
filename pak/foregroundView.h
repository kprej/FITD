#pragma once

#include "view.h"

#include <bgfx/bgfx.h>

#include <memory>

class foregroundView_t : public view_t
{
public:
    ~foregroundView_t ();
    foregroundView_t ();

protected:
    friend class bgfxHandle_t;
    void init () final;
    void startFrame () final;

private:
    void _shutdown () final;

    class private_t;
    std::unique_ptr<private_t> m_d;
};
