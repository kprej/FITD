#include "body.h"
#include "buffer.h"
#include "osystem.h"
#include "types.h"

#include <plog/Log.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <cassert>
using namespace std;

namespace
{
enum infoFlags_t
{
    TRI = 1,
    ANIM = 2,
    TORTUE = 4,
    OPTIMISE = 8,
};
}

class body_t::private_t
{
public:
    ~private_t ()
    {
        bgfx::destroy (vBuffer);
        bgfx::destroy (iBuffer);
    }
    private_t () {}

    uint16_t flags;
    ZVStruct zv;
    vector<byte> scratchBuffer;
    vector<point_t> vertices;
    vector<uint16_t> groupOrder;
    vector<sGroup> groups;
    vector<sPrimitive> primitives;

    vector<raw_t> raw;
    vector<uint16_t> points;

    bgfx::VertexBufferHandle vBuffer;
    bgfx::IndexBufferHandle iBuffer;

    vector<byte> palette;

    glm::quat rotation;
    glm::vec3 position;
};

body_t::~body_t () = default;

body_t::body_t ()
    : m_d (make_shared<private_t> ())
{
}

void body_t::parseData (vector<byte> const &data_)
{
    buffer_t bodyBuffer (data_, data_.size ());

    m_d->flags = bodyBuffer.get<uint16_t> ();

    m_d->zv.ZVX1 = bodyBuffer.get<uint16_t> ();
    m_d->zv.ZVX2 = bodyBuffer.get<uint16_t> ();
    m_d->zv.ZVY1 = bodyBuffer.get<uint16_t> ();
    m_d->zv.ZVY2 = bodyBuffer.get<uint16_t> ();
    m_d->zv.ZVZ1 = bodyBuffer.get<uint16_t> ();
    m_d->zv.ZVZ2 = bodyBuffer.get<uint16_t> ();

    m_d->scratchBuffer.resize (bodyBuffer.get<uint16_t> ());

    for (auto &buffer : m_d->scratchBuffer)
        buffer = bodyBuffer.get<byte> ();

    m_d->vertices.resize (bodyBuffer.get<uint16_t> ());
    for (auto &vert : m_d->vertices)
    {
        vert.x = bodyBuffer.get<int16_t> ();
        vert.y = bodyBuffer.get<int16_t> ();
        vert.z = bodyBuffer.get<int16_t> ();
        m_d->raw.push_back ({{float (vert.x), float (vert.y), float (vert.z)}, 0});
    }

    if (m_d->flags & ANIM)
    {
        uint16_t numGroups = bodyBuffer.get<uint16_t> ();

        m_d->groupOrder.resize (numGroups);
        m_d->groups.resize (numGroups);

        if (m_d->flags & OPTIMISE) // AITD2+
        {
            for (int i = 0; i < numGroups; i++)
            {
                uint16_t offset = bodyBuffer.get<uint16_t> ();
                assert (offset % 0x18 == 0);
                m_d->groupOrder.push_back (offset / 0x18);
            }

            for (auto &group : m_d->groups)
            {
                group.m_start = bodyBuffer.get<int16_t> () / 6;
                group.m_numVertices = bodyBuffer.get<int16_t> ();
                group.m_baseVertices = bodyBuffer.get<int16_t> () / 6;
                group.m_orgGroup = bodyBuffer.get<int8_t> ();
                group.m_numGroup = bodyBuffer.get<int8_t> ();

                group.m_state.m_type = bodyBuffer.get<int16_t> ();

                group.m_state.m_delta[0] = bodyBuffer.get<int16_t> ();
                group.m_state.m_delta[1] = bodyBuffer.get<int16_t> ();
                group.m_state.m_delta[2] = bodyBuffer.get<int16_t> ();

                group.m_state.m_rotateDelta[0] = bodyBuffer.get<int16_t> ();
                group.m_state.m_rotateDelta[1] = bodyBuffer.get<int16_t> ();
                group.m_state.m_rotateDelta[2] = bodyBuffer.get<int16_t> ();

                bodyBuffer.seekg (2);
            }
        }
        else
        {
            for (auto &order : m_d->groupOrder)
            {
                uint16_t offset = bodyBuffer.get<uint16_t> ();
                assert (offset % 0x10 == 0);
                order = offset / 0x10;
            }

            for (auto &group : m_d->groups)
            {
                group.m_start = bodyBuffer.get<int16_t> () / 6;
                group.m_numVertices = bodyBuffer.get<int16_t> ();
                group.m_baseVertices = bodyBuffer.get<int16_t> () / 6;
                group.m_orgGroup = bodyBuffer.get<int8_t> ();
                group.m_numGroup = bodyBuffer.get<int8_t> ();
                group.m_state.m_type = bodyBuffer.get<int16_t> ();
                group.m_state.m_delta[0] = bodyBuffer.get<int16_t> ();
                group.m_state.m_delta[1] = bodyBuffer.get<int16_t> ();
                group.m_state.m_delta[2] = bodyBuffer.get<int16_t> ();
            }
        }
    }

    uint16_t numPrimitives = bodyBuffer.get<uint16_t> ();
    m_d->primitives.resize (numPrimitives);

    for (auto &primitive : m_d->primitives)
    {
        primitive.m_type = (primitiveType_t)bodyBuffer.get<uint8_t> ();

        switch (primitive.m_type)
        {
        case Line:
            primitive.m_subType = bodyBuffer.get<uint8_t> ();
            primitive.m_color = bodyBuffer.get<uint8_t> ();
            primitive.m_even = bodyBuffer.get<uint8_t> ();
            primitive.m_points.resize (2);
            for (int j = 0; j < primitive.m_points.size (); j++)
            {
                uint16_t point = bodyBuffer.get<uint16_t> () / 6;
                primitive.m_points[j] = point;
                m_d->points.push_back (primitive.m_points[j]);
                m_d->raw[point].color = primitive.m_color;
            }
            break;
        case Poly:
            primitive.m_points.resize (bodyBuffer.get<uint8_t> ());
            primitive.m_subType = bodyBuffer.get<uint8_t> ();
            primitive.m_color = bodyBuffer.get<uint8_t> ();
            for (int j = 0; j < primitive.m_points.size (); j++)
            {
                uint16_t point = bodyBuffer.get<uint16_t> () / 6;
                primitive.m_points[j] = point;
                m_d->points.push_back (primitive.m_points[j]);
                m_d->raw[point].color = primitive.m_color;
            }

            if (primitive.m_points.size () > 3)
            {
                m_d->points.push_back (primitive.m_points[0]);
                m_d->points.push_back (primitive.m_points[2]);
            }

            break;
        case Point:
        case BigPoint:
        case Zixel:
            primitive.m_subType = bodyBuffer.get<uint8_t> ();
            primitive.m_color = bodyBuffer.get<uint8_t> ();
            primitive.m_even = bodyBuffer.get<uint8_t> ();
            primitive.m_points.resize (1);
            for (int j = 0; j < primitive.m_points.size (); j++)
            {
                uint16_t point = bodyBuffer.get<uint16_t> () / 6;
                primitive.m_points[j] = point;
                m_d->points.push_back (primitive.m_points[j]);
                m_d->raw[point].color = primitive.m_color;
            }
            break;
        case Sphere:
            primitive.m_subType = bodyBuffer.get<uint8_t> ();
            primitive.m_color = bodyBuffer.get<uint8_t> ();
            primitive.m_even = bodyBuffer.get<uint8_t> ();
            primitive.m_size = bodyBuffer.get<uint16_t> ();
            primitive.m_points.resize (1);
            for (int j = 0; j < primitive.m_points.size (); j++)
            {
                uint16_t point = bodyBuffer.get<uint16_t> () / 6;
                primitive.m_points[j] = point;
                m_d->points.push_back (primitive.m_points[j]);
                m_d->raw[point].color = primitive.m_color;
            }
            break;
        case PolyTexture8:
            PLOGD << "PLY TEX 8";
            primitive.m_points.resize (bodyBuffer.get<uint8_t> ());
            primitive.m_subType = bodyBuffer.get<uint8_t> ();
            primitive.m_color = bodyBuffer.get<uint8_t> ();
            for (int j = 0; j < primitive.m_points.size (); j++)
            {
                uint16_t point = bodyBuffer.get<uint16_t> () / 6;
                primitive.m_points[j] = point;
                m_d->points.push_back (primitive.m_points[j]);
                m_d->raw[point].color = primitive.m_color;
            }
            break;
        case PolyTexture9:
        case PolyTexture10:
            PLOGD << "PLY TEX 10/9";
            primitive.m_points.resize (bodyBuffer.get<uint8_t> ());
            primitive.m_subType = bodyBuffer.get<uint8_t> ();
            primitive.m_color = bodyBuffer.get<uint8_t> ();
            for (int j = 0; j < primitive.m_points.size (); j++)
            {
                uint16_t point = bodyBuffer.get<uint16_t> () / 6;
                primitive.m_points[j] = point;
                m_d->points.push_back (primitive.m_points[j]);
                m_d->raw[point].color = primitive.m_color;
            }
            // load UVS?
            for (int j = 0; j < primitive.m_points.size (); j++)
            {
                bodyBuffer.get<uint8_t> ();
                bodyBuffer.get<uint8_t> ();
            }
            break;
        default:
            assert (0);
            break;
        }
    }

    m_d->vBuffer = bgfx::createVertexBuffer (
        bgfx::makeRef (m_d->raw.data (), sizeof (raw_t) * m_d->raw.size ()),
        GS ()->handle.bodyVertexLayout ());

    m_d->iBuffer = bgfx::createIndexBuffer (
        bgfx::makeRef (m_d->points.data (), sizeof (uint16_t) * m_d->points.size ()));
}

void body_t::rotateX (float x_)
{
    m_d->rotation *= glm::angleAxis (glm::radians (x_), glm::vec3 (1.0f, 0.0f, 0.0f));
}

void body_t::rotateY (float y_)
{
    m_d->rotation *= glm::angleAxis (glm::radians (y_), glm::vec3 (0.0f, 1.0f, 0.0f));
}

void body_t::rotateZ (float z_)
{
    m_d->rotation *= glm::angleAxis (glm::radians (z_), glm::vec3 (0.0f, 0.0f, 1.0f));
}

void body_t::pos (float x_, float y_, float z_)
{
    m_d->position = glm::vec3 (x_, y_, z_);
}

bgfx::VertexBufferHandle const &body_t::vertexBuffer () const
{
    return m_d->vBuffer;
}

bgfx::IndexBufferHandle const &body_t::indexBuffer () const
{
    return m_d->iBuffer;
}

glm::mat4 body_t::transform ()
{
    glm::mat4 mat (1.0f);

    mat = glm::translate (mat, m_d->position) * glm::toMat4 (m_d->rotation);

    return mat;
}
