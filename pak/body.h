#pragma once

#include <bgfx/bgfx.h>

#include <memory>
#include <vector>

class body_t
{
public:
    struct raw_t
    {
        float pos[3];
        float texCoord[2];
    };

    ~body_t ();
    body_t ();

    void parseData (std::vector<std::byte> const &data_);

    bgfx::VertexBufferHandle const &vertexBuffer () const;

private:
    class private_t;
    std::shared_ptr<private_t> m_d;
};
