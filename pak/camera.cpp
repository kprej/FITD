#include "camera.h"
#include "osystem.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <imgui.h>

using namespace std;

class camera_t::private_t
{
public:
    ~private_t () = default;
    private_t ()
        : up (0.f, 1.f, 0.f)
        , eye (0.0f, 0.f, 1.0f)
        , center (0.f, 0.f, 0.f)
        , fov (60.f)
        , near (0.1f)
        , far (10000.f)
        , right (320)
        , bottom (200)
        , viewPos (0, 0)
        , viewSize (320, 200)
    {
    }

    glm::vec3 up;
    glm::vec3 eye;
    glm::vec3 center;

    float fov;
    float near;
    float far;

    float left;
    float right;
    float top;
    float bottom;

    glm::vec2 viewPos;
    glm::vec2 viewSize;
};

camera_t::~camera_t () = default;

camera_t::camera_t ()
    : m_d (make_unique<private_t> ())
{
}

void camera_t::init ()
{
    GS ()->debug.draw.connect<&camera_t::drawDebug> (this);
}

void camera_t::setViewPos (glm::vec2 const &vec_)
{
    m_d->viewPos = vec_;
}

void camera_t::setViewSize (glm::vec2 const &vec_)
{
    m_d->viewSize = vec_;
}

void camera_t::setFOV (float const &fov_)
{
    m_d->fov = fov_;
}

void camera_t::setPos (glm::vec3 const &pos_)
{
    m_d->eye = pos_;
}

void camera_t::lookAt (glm::vec3 const &pos_)
{
    m_d->center = pos_;
}

glm::mat4 camera_t::view ()
{
    auto const look = glm::lookAt (m_d->eye, m_d->center, m_d->up);

    if (glm::isnan (look[0][0]))
        return glm::mat4 (1.0f);

    return look;
}

glm::vec2 camera_t::viewPos () const
{
    return m_d->viewPos;
}

glm::vec2 camera_t::viewSize () const
{
    return m_d->viewSize;
}

glm::mat4 camera_t::projection ()
{
    return glm::perspective (
        glm::radians (m_d->fov), m_d->right / m_d->bottom, m_d->near, m_d->far);
}

void camera_t::drawDebug ()
{
    if (ImGui::Begin ("Camera"))
    {
        ImGui::SliderFloat ("FOV", &m_d->fov, 0.f, 180.f);
        ImGui::InputFloat ("Near", &m_d->near, 1.0f, 1000.f);
        ImGui::InputFloat ("Far", &m_d->far, 1.f, 1000.f);
        ImGui::InputFloat3 ("Eye", glm::value_ptr (m_d->eye));
        ImGui::InputFloat3 ("Center", glm::value_ptr (m_d->center));
        ImGui::InputFloat ("right", &m_d->right, 1.f, 2000);
        ImGui::InputFloat ("bottom", &m_d->bottom, 1.f, 2000);
        ImGui::InputFloat2 ("View POS", glm::value_ptr (m_d->viewPos));
        ImGui::InputFloat2 ("View Size", glm::value_ptr (m_d->viewSize));

        if (ImGui::TreeNode ("View Matrix"))
        {
            if (ImGui::BeginTable ("View", 4))
            {
                auto const v = view ();
                for (int row = 0; row < 4; row++)
                {
                    ImGui::TableNextRow ();
                    for (int column = 0; column < 4; column++)
                    {
                        ImGui::TableSetColumnIndex (column);
                        ImGui::Text ("%f", v[row][column]);
                    }
                }
                ImGui::EndTable ();
            }
            ImGui::TreePop ();
        }

        if (ImGui::TreeNode ("Projection Matrix"))
        {
            if (ImGui::BeginTable ("Projection", 4))
            {
                auto const proj = projection ();
                for (int row = 0; row < 4; row++)
                {
                    ImGui::TableNextRow ();
                    for (int column = 0; column < 4; column++)
                    {
                        ImGui::TableSetColumnIndex (column);
                        ImGui::Text ("%f", proj[row][column]);
                    }
                }
                ImGui::EndTable ();
            }
            ImGui::TreePop ();
        }
    }
    ImGui::End ();
}
