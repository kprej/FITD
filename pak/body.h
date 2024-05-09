#pragma once

#include <bgfx/bgfx.h>
#include <glm/mat4x4.hpp>

#include <memory>
#include <vector>

class body_t
{
public:
    struct raw_t
    {
        float pos[3];
        float color;
    };

    ~body_t ();
    body_t ();

    void parseData (std::vector<std::byte> const &data_);

    void rotateX (float x_);
    void rotateY (float y_);
    void rotateZ (float z_);

    void pos (float x_, float y_, float z_);

    bgfx::VertexBufferHandle const &vertexBuffer () const;
    bgfx::IndexBufferHandle const &indexBuffer () const;

    glm::mat4 transform ();

private:
    class private_t;
    std::shared_ptr<private_t> m_d;
};
