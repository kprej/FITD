#include "body.h"
#include "buffer.h"
#include "types.h"

#include <plog/Log.h>

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
    ~private_t () = default;
    private_t () {}

    uint16_t flags;
    ZVStruct zv;
    vector<byte> scratchBuffer;
    vector<point_t> vertices;
    vector<uint16_t> groupOrder;
    vector<sGroup> groups;
    vector<sPrimitive> primitives;

    vector<raw_t> raw;

    bgfx::VertexBufferHandle buffer;
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

        m_d->raw.push_back ({{float (vert.x), float (vert.y), float (vert.z)}, {0, 0}});

        PLOGD << vert.x << " " << vert.y << " " << vert.z;
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
                primitive.m_points[j] = bodyBuffer.get<uint16_t> () / 6;
            }
            break;
        case Poly:
            primitive.m_points.resize (bodyBuffer.get<uint8_t> ());
            primitive.m_subType = bodyBuffer.get<uint8_t> ();
            primitive.m_color = bodyBuffer.get<uint8_t> ();
            for (int j = 0; j < primitive.m_points.size (); j++)
            {
                primitive.m_points[j] = bodyBuffer.get<uint16_t> () / 6;
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
                primitive.m_points[j] = bodyBuffer.get<uint16_t> () / 6;
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
                primitive.m_points[j] = bodyBuffer.get<uint16_t> () / 6;
            }
            break;
        case PolyTexture8:
            primitive.m_points.resize (bodyBuffer.get<uint8_t> ());
            primitive.m_subType = bodyBuffer.get<uint8_t> ();
            primitive.m_color = bodyBuffer.get<uint8_t> ();
            for (int j = 0; j < primitive.m_points.size (); j++)
            {
                primitive.m_points[j] = bodyBuffer.get<uint16_t> () / 6;
            }
            break;
        case PolyTexture9:
        case PolyTexture10:
            primitive.m_points.resize (bodyBuffer.get<uint8_t> ());
            primitive.m_subType = bodyBuffer.get<uint8_t> ();
            primitive.m_color = bodyBuffer.get<uint8_t> ();
            for (int j = 0; j < primitive.m_points.size (); j++)
            {
                primitive.m_points[j] = bodyBuffer.get<uint16_t> () / 6;
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

    bgfx::VertexLayout layout;
    layout.begin ()
        .add (bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add (bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end ();

    m_d->buffer = bgfx::createVertexBuffer (
        bgfx::makeRef (m_d->raw.data (), sizeof (raw_t) * m_d->raw.size ()), layout);
}

bgfx::VertexBufferHandle const &body_t::vertexBuffer () const
{
    return m_d->buffer;
}
