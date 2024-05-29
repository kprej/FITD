#pragma once

#include <bgfx/bgfx.h>

#include <glm/glm.hpp>

#include <spimpl.h>

#include <optional>
#include <vector>

class texture_t
{
public:
    enum preset_t
    {
        FULLSCREEN,
        PALETTE,
    };

    ~texture_t ();
    texture_t ();

    texture_t (glm::tvec2<uint16_t> const &size_);
    texture_t (preset_t preset_);
    texture_t (preset_t preset_, std::vector<std::byte> const &data_);

    glm::tvec2<uint16_t> const &size () const;

    bgfx::TextureHandle const &handle () const;
    uint8_t palette () const;

    void update (std::vector<std::byte> const &data_,
                 uint16_t offset_ = 0,
                 uint16_t range_ = 0);

    void fill (uint8_t color_, glm::tvec4<uint16_t> rect_);
    void fill (uint8_t color_);

    void clear ();

    void setPalette (uint8_t palette_);

    bool isValid () const;

private:
    void init ();

    class private_t;
    spimpl::impl_ptr<private_t> m_d;
};

struct transientTexture_t
{
    texture_t texture;
    bgfx::TransientVertexBuffer buffer;
};
