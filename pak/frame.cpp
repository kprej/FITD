#include "frame.h"
#include "osystem.h"
#include "texture.h"

#include <bgfx/bgfx.h>

#include <plog/Log.h>

using namespace std;

namespace
{
texture_t getTexture (vector<byte> const &buffer_, uint8_t index_)
{
    auto ptr = buffer_.begin ();

    ptr += (2 * index_);

    auto skip = (uint8_t (*ptr)) | (uint8_t (*(ptr + 1)) << 8);
    ptr = buffer_.begin () + skip + 4;

    auto width = (uint8_t (*ptr)) | (uint8_t (*(ptr + 1)) << 8);
    ptr += 2;

    auto height = (uint8_t (*ptr)) | (uint8_t (*(ptr + 1)) << 8);
    ptr += 2;

    PLOGD << width << " " << height;
    vector<byte> out;

    for (auto i = 0; i < width * height; ++i)
        out.push_back (*(ptr++));

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
};

frame_t::~frame_t () = default;

frame_t::frame_t ()
    : m_d (make_unique<private_t> ())
{
}

void frame_t::setTexture (vector<byte> const &data_)
{
    GS ()->textures[0] = getTexture (data_, 0);
    GS ()->textures[0].setPalette (255);

    //  m_d->topRight = getTexture (data_, 1);
    //  m_d->bottomLeft = getTexture (data_, 2);
    //  m_d->bottomRight = getTexture (data_, 3);

    // m_d->top = getTexture (data_, 4);
    // m_d->bottom = getTexture (data_, 5);
    // m_d->left = getTexture (data_, 6);
    // m_d->right = getTexture (data_, 7);

    // m_d->inTheDark = getTexture (data_, 8);
}

void frame_t::render (glm::tvec4<uint16_t> const &rect_)
{
    bgfx::TransientVertexBuffer transientVertex;
    bgfx::allocTransientVertexBuffer (
        &transientVertex, 6, GS ()->handle.textureVertexLayout ());

    textureVert_t *verts = (textureVert_t *)transientVertex.data;

    topLeft (verts, rect_);

    GS ()->handle.drawToForeground (transientVertex, GS ()->textures.at (0));
}

void frame_t::topLeft (textureVert_t *verts_, glm::tvec4<uint16_t> const &rect_)
{

    auto tex = GS ()->textures.at (0);
    // o.o
    // . .
    // *.o
    verts_->position[0] = rect_.x;
    verts_->position[1] = rect_.y;
    verts_->position[2] = 0;

    verts_->texCoord[0] = 0;
    verts_->texCoord[1] = 1;

    ++verts_;

    // o.o
    // . .
    // o.*
    verts_->position[0] = rect_.x + tex.size ().x;
    verts_->position[1] = rect_.y;
    verts_->position[2] = 0;

    verts_->texCoord[0] = 1;
    verts_->texCoord[1] = 1;

    ++verts_;

    // o.*
    // . .
    // o.o
    verts_->position[0] = rect_.x + tex.size ().x;
    verts_->position[1] = rect_.y + tex.size ().y;
    verts_->position[2] = 0;

    verts_->texCoord[0] = 1;
    verts_->texCoord[1] = 0;

    ++verts_;

    // o.o
    // . .
    // *.o
    verts_->position[0] = rect_.x;
    verts_->position[1] = rect_.y;
    verts_->position[2] = 0;

    verts_->texCoord[0] = 0;
    verts_->texCoord[1] = 1;

    ++verts_;

    // *.o
    // . .
    // o.o
    verts_->position[0] = rect_.x;
    verts_->position[1] = rect_.y + tex.size ().y;
    verts_->position[2] = 0;

    verts_->texCoord[0] = 0;
    verts_->texCoord[1] = 0;

    ++verts_;

    // o.*
    // . .
    // o.o
    verts_->position[0] = rect_.x + tex.size ().x;
    verts_->position[1] = rect_.y + tex.size ().y;
    verts_->position[2] = 0;

    verts_->texCoord[0] = 1;
    verts_->texCoord[1] = 0;

    ++verts_;
}
