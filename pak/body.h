#pragma once
#include "texture.h"
#include "vertTypes.h"

#include <bgfx/bgfx.h>
#include <glm/mat4x4.hpp>
#include <spimpl.h>

#include <optional>
#include <vector>

class body_t
{
public:
    ~body_t ();
    body_t ();

    void parseData (std::vector<std::byte> const &data_);

    void rotateX (float x_);
    void rotateY (float y_);
    void rotateZ (float z_);

    void setPos (float x_, float y_, float z_);

    void updateScale (float scale_);
    void setScale (float scale_);

    void setPalette (texture_t const &texture_);

    std::optional<texture_t> const &palette () const;

    bgfx::VertexBufferHandle const &vertexBuffer () const;
    bgfx::IndexBufferHandle const &indexBuffer () const;
    std::vector<primitive_t> const &primitives () const;

    glm::vec3 pos () const;
    glm::vec3 center () const;

    glm::mat4 transform () const;

    glm::mat4 boundingBox () const;

    void draw ();

    void debug ();

private:
    class private_t;
    spimpl::impl_ptr<private_t> m_d;
};
