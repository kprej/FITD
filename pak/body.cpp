#include "body.h"
#include "buffer.h"
#include "osystem.h"
#include "types.h"

#include <plog/Log.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

#include <imgui.h>

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
    private_t ()
        : position (0, 0, 0)
        , scale (1)
        , rotation (0, 0, 0)
        , min (0, 0, 0)
        , max (0, 0, 0)
        , size (0, 0, 0)
        , center (0, 0, 0)
    {
    }

    uint16_t flags;
    ZVStruct zv;
    vector<byte> scratchBuffer;
    vector<point_t> vertices;
    vector<uint16_t> groupOrder;
    vector<sGroup> groups;
    vector<sPrimitive> primitives;

    vector<rawBody_t> rawBody;
    vector<uint16_t> points;

    bgfx::VertexBufferHandle vBuffer;
    bgfx::IndexBufferHandle iBuffer;
    vector<primitive_t> prims;

    vector<byte> palette;

    glm::vec3 rotation;
    glm::vec3 position;
    float scale;

    glm::vec3 min;
    glm::vec3 max;
    glm::vec3 size;
    glm::vec3 center;

    glm::mat4 boundingBox;
};

body_t::~body_t () = default;

body_t::body_t ()
    : m_d (make_shared<private_t> ())
{
}

void body_t::parseData (vector<byte> const &data_)
{
    buffer_t bodyBuffer (data_);

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
        vert.x = bodyBuffer.get<int16_t> () / 100;
        vert.y = -1 * bodyBuffer.get<int16_t> () / 100;
        vert.z = bodyBuffer.get<int16_t> () / 100;
        m_d->rawBody.push_back ({float (vert.x), float (vert.y), float (vert.z)});

        if (vert.x < m_d->min.x)
            m_d->min.x = vert.x;
        if (vert.x > m_d->max.x)
            m_d->max.x = vert.x;
        if (vert.y < m_d->min.y)
            m_d->min.y = vert.y;
        if (vert.y > m_d->max.y)
            m_d->max.y = vert.y;
        if (vert.z < m_d->min.z)
            m_d->min.z = vert.z;
        if (vert.z > m_d->max.z)
            m_d->max.z = vert.z;
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

    uint16_t index = 0;
    for (auto &primitive : m_d->primitives)
    {
        primitive.m_type = (primitiveType_t)bodyBuffer.get<uint8_t> ();

        primitive_t prim;
        prim.start = index;

        switch (primitive.m_type)
        {
        case Line:
            primitive.m_subType = bodyBuffer.get<uint8_t> ();
            prim.color = bodyBuffer.get<uint8_t> ();
            primitive.m_even = bodyBuffer.get<uint8_t> ();
            primitive.m_points.resize (2);
            prim.size = 2;
            index += 2;
            for (int j = 0; j < primitive.m_points.size (); j++)
            {
                uint16_t point = bodyBuffer.get<uint16_t> () / 6;
                primitive.m_points[j] = point;
                m_d->points.push_back (primitive.m_points[j]);
            }
            break;
        case Poly:
            primitive.m_points.resize (bodyBuffer.get<uint8_t> ());
            primitive.m_subType = bodyBuffer.get<uint8_t> ();
            prim.color = bodyBuffer.get<uint8_t> ();
            prim.size = primitive.m_points.size ();
            index += prim.size;
            for (int j = 0; j < primitive.m_points.size (); j++)
            {
                uint16_t point = bodyBuffer.get<uint16_t> () / 6;
                primitive.m_points[j] = point;
                m_d->points.push_back (primitive.m_points[j]);
            }

            if (primitive.m_points.size () > 3)
            {
                m_d->points.push_back (primitive.m_points[0]);
                m_d->points.push_back (primitive.m_points[2]);
                index += 2;
                prim.size += 2;
            }

            break;
        case Point:
        case BigPoint:
        case Zixel:
            primitive.m_subType = bodyBuffer.get<uint8_t> ();
            prim.color = bodyBuffer.get<uint8_t> ();
            primitive.m_even = bodyBuffer.get<uint8_t> ();
            primitive.m_points.resize (1);
            for (int j = 0; j < primitive.m_points.size (); j++)
            {
                uint16_t point = bodyBuffer.get<uint16_t> () / 6;
                primitive.m_points[j] = point;
                m_d->points.push_back (primitive.m_points[j]);
            }
            break;
        case Sphere:
            primitive.m_subType = bodyBuffer.get<uint8_t> ();
            prim.color = bodyBuffer.get<uint8_t> ();
            primitive.m_even = bodyBuffer.get<uint8_t> ();
            primitive.m_size = bodyBuffer.get<uint16_t> ();
            primitive.m_points.resize (1);
            for (int j = 0; j < primitive.m_points.size (); j++)
            {
                uint16_t point = bodyBuffer.get<uint16_t> () / 6;
                primitive.m_points[j] = point;
                m_d->points.push_back (primitive.m_points[j]);
            }
            break;
        case PolyTexture8:
            PLOGD << "PLY TEX 8";
            primitive.m_points.resize (bodyBuffer.get<uint8_t> ());
            primitive.m_subType = bodyBuffer.get<uint8_t> ();
            prim.color = bodyBuffer.get<uint8_t> ();
            for (int j = 0; j < primitive.m_points.size (); j++)
            {
                uint16_t point = bodyBuffer.get<uint16_t> () / 6;
                primitive.m_points[j] = point;
                m_d->points.push_back (primitive.m_points[j]);
            }
            break;
        case PolyTexture9:
        case PolyTexture10:
            PLOGD << "PLY TEX 10/9";
            primitive.m_points.resize (bodyBuffer.get<uint8_t> ());
            primitive.m_subType = bodyBuffer.get<uint8_t> ();
            prim.color = bodyBuffer.get<uint8_t> ();
            for (int j = 0; j < primitive.m_points.size (); j++)
            {
                uint16_t point = bodyBuffer.get<uint16_t> () / 6;
                primitive.m_points[j] = point;
                m_d->points.push_back (primitive.m_points[j]);
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

        m_d->prims.push_back (prim);
    }

    m_d->size = m_d->max - m_d->min;
    m_d->center = (m_d->min + m_d->max) / 2.f;

    PLOGD << m_d->center.x << " " << m_d->center.y;

    m_d->boundingBox = glm::translate (glm::mat4 (1), m_d->center) *
                       glm::scale (glm::mat4 (1), m_d->size);

    m_d->vBuffer = bgfx::createVertexBuffer (
        bgfx::makeRef (m_d->rawBody.data (), sizeof (rawBody_t) * m_d->rawBody.size ()),
        GS ()->handle.bodyVertexLayout ());

    m_d->iBuffer = bgfx::createIndexBuffer (
        bgfx::makeRef (m_d->points.data (), sizeof (uint16_t) * m_d->points.size ()));
}

void body_t::rotateX (float x_)
{
    m_d->rotation.x += x_;
}

void body_t::rotateY (float y_)
{
    m_d->rotation.y += y_;
}

void body_t::rotateZ (float z_)
{
    m_d->rotation.z += z_;
}

void body_t::setPos (float x_, float y_, float z_)
{
    m_d->position = glm::vec3 (x_, y_, z_);
}

void body_t::updateScale (float scale_)
{
    m_d->scale += scale_;
}

void body_t::setScale (float scale_)
{
    m_d->scale = scale_;
}

bgfx::VertexBufferHandle const &body_t::vertexBuffer () const
{
    return m_d->vBuffer;
}

bgfx::IndexBufferHandle const &body_t::indexBuffer () const
{
    return m_d->iBuffer;
}

vector<primitive_t> const &body_t::primitives () const
{
    return m_d->prims;
}

glm::vec3 body_t::pos () const
{
    return m_d->position;
}

glm::vec3 body_t::center () const
{
    return m_d->center + m_d->position;
}

glm::mat4 body_t::transform () const
{
    glm::mat4 mat (1.0f);
    glm::quat rotation = glm::angleAxis (0.f, glm::vec3 (0.f, 0.f, 0.f));

    rotation *=
        glm::angleAxis (glm::radians (m_d->rotation.x), glm::vec3 (1.0f, 0.0f, 0.0f));
    rotation *=
        glm::angleAxis (glm::radians (m_d->rotation.y), glm::vec3 (0.0f, 1.0f, 0.0f));
    rotation *=
        glm::angleAxis (glm::radians (m_d->rotation.z), glm::vec3 (0.0f, 0.0f, 1.0f));

    mat = glm::translate (mat, m_d->position) * glm::toMat4 (rotation) *
          glm::scale (mat, glm::vec3 {m_d->scale, m_d->scale, m_d->scale});

    return mat;
}

glm::mat4 body_t::boundingBox () const
{
    return m_d->boundingBox;
}

void body_t::debug ()
{
    ImGui::InputFloat3 ("Position", glm::value_ptr (m_d->position));
    ImGui::InputFloat3 ("Rotation", glm::value_ptr (m_d->rotation));
    ImGui::InputFloat ("Scale", &m_d->scale);
}
