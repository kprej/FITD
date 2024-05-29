#pragma once

#include "vertTypes.h"

#include <glm/glm.hpp>

#include <memory>
#include <vector>

class frame_t
{
public:
    ~frame_t ();
    frame_t ();

    void setTexture (std::vector<std::byte> const &data_);

    void render (glm::tvec4<uint16_t> const &rect_);

private:
    void topLeft (textureVert_t *verts_, glm::tvec4<uint16_t> const &rect_);

    class private_t;
    std::unique_ptr<private_t> m_d;
};
