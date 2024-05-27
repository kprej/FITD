#pragma once

#include "frame.h"
#include "buffer.h"
#include "osystem.h"
#include "texture.h"

#include <bgfx/bgfx.h>

using namespace std;

namespace
{
texture_t getTexture (buffer_t &buffer_, uint8_t index_)
{
    buffer_.seekg (2 * index_, buffer_t::beg);

    auto skip = buffer_.get<uint16_t> ();

    buffer_.seekg (skip + 4, buffer_t::beg);

    auto width = buffer_.get<uint16_t> ();
    auto height = buffer_.get<uint16_t> ();

    vector<byte> out;

    for (auto i = 0; i < width * height; ++i)
    {
        out.push_back (buffer_.get<byte> ());
    }

    texture_t outT ({width, height});
    outT.update (out);

    return outT;
}
} // namespace

class frame_t::private_t
{
public:
    ~private_t () {}
    private_t () {}

    texture_t topLeft;
    texture_t topRight;
    texture_t bottomLeft;
    texture_t bottomRight;

    texture_t top;
    texture_t left;
    texture_t right;
    texture_t bottom;
    texture_t inTheDark;
};

frame_t::~frame_t () = default;

frame_t::frame_t ()
{
}

void frame_t::setTexture (vector<byte> const &data_)
{
    buffer_t textureBuff (data_);
    m_d->topLeft = getTexture (textureBuff, 0);
    m_d->topRight = getTexture (textureBuff, 1);
    m_d->bottomLeft = getTexture (textureBuff, 2);
    m_d->bottomRight = getTexture (textureBuff, 3);

    m_d->top = getTexture (textureBuff, 4);
    m_d->bottom = getTexture (textureBuff, 5);
    m_d->left = getTexture (textureBuff, 6);
    m_d->right = getTexture (textureBuff, 7);

    m_d->inTheDark = getTexture (textureBuff, 8);

    initCorners ();
}

void frame_t::render (glm::tvec4<uint16_t> const &rect_)
{
    bgfx::TransientVertexBuffer transientVertex;
    bgfx::allocTransientVertexBuffer (
        &transientVertex, 16, GS ()->handle.textureVertexLayout ());

    textureVert_t *verts = (textureVert_t *)transientVertex.data;

    auto i = verts;
    for (auto v = 0; v < 0)
    {
        auto const character = m_d->characters[c];

        // o.o
        // . .
        // *.o
        i->position[0] = currentPos.x;
        i->position[1] = currentPos.y;
        i->position[2] = 0;

        i->texCoord[0] = character.bottomLeftCorner[0];
        i->texCoord[1] = character.bottomLeftCorner[1];

        ++i;

        // o.o
        // . .
        // o.*
        i->position[0] = currentPos.x + character.width;
        i->position[1] = currentPos.y;
        i->position[2] = 0;

        i->texCoord[0] = character.topRightCorner[0];
        i->texCoord[1] = character.bottomLeftCorner[1];

        ++i;

        // o.*
        // . .
        // o.o
        i->position[0] = currentPos.x + character.width;
        i->position[1] = currentPos.y + character.height;
        i->position[2] = 0;

        i->texCoord[0] = character.topRightCorner[0];
        i->texCoord[1] = character.topRightCorner[1];

        ++i;

        // o.o
        // . .
        // *.o
        i->position[0] = currentPos.x;
        i->position[1] = currentPos.y;
        i->position[2] = 0;

        i->texCoord[0] = character.bottomLeftCorner[0];
        i->texCoord[1] = character.bottomLeftCorner[1];

        ++i;

        // *.o
        // . .
        // o.o
        i->position[0] = currentPos.x;
        i->position[1] = currentPos.y + character.height;
        i->position[2] = 0;

        i->texCoord[0] = character.bottomLeftCorner[0];
        i->texCoord[1] = character.topRightCorner[1];

        ++i;

        // o.*
        // . .
        // o.o
        i->position[0] = currentPos.x + character.width;
        i->position[1] = currentPos.y + character.height;
        i->position[2] = 0;

        i->texCoord[0] = character.topRightCorner[0];
        i->texCoord[1] = character.topRightCorner[1];

        ++i;

        currentPos.x += character.width;
    }

    GS ()->handle.renderText (transientVertex);
}
