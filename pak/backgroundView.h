#pragma once

#include "view.h"

#include <bgfx/bgfx.h>
#include <glm/glm.hpp>

#include <memory>

class bgfxHandle_t;
class backgroundView_t : public view_t
{
public:
    ~backgroundView_t ();
    backgroundView_t ();

protected:
    friend class bgfxHandle_t;
    void init () final;
    void startFrame () final;

private:
    void _shutdown () final;

    class private_t;
    std::unique_ptr<private_t> m_d;
};
