#pragma once

#include <bgfx/bgfx.h>
#include <glm/glm.hpp>

#include <memory>
#include <vector>

class bgfxHandle_t;
class backgroundView_t
{
public:
    ~backgroundView_t ();
    backgroundView_t ();

    void update (std::vector<std::byte> const &texture_, int offset_);
    void blackout (glm::tvec2<unsigned> const &pointA_,
                   glm::tvec2<unsigned> const &pointB_);

protected:
    friend class bgfxHandle_t;
    void init ();
    void startFrame ();
    void render ();
    void shutdown ();
    bgfx::TextureHandle texture ();

private:
    class private_t;
    std::unique_ptr<private_t> m_d;
};
