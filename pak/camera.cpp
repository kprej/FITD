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
        , eye (0.f, 0.f, -65.f)
        , center (0.f, 0.f, 0.f)
        , fov (60.f)
        , near (0.1)
        , far (1000.f)
    {
    }

    glm::vec3 up;
    glm::vec3 eye;
    glm::vec3 center;

    float fov;
    float near;
    float far;
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

glm::mat4 camera_t::view ()
{
    return glm::lookAt (m_d->eye, m_d->center, m_d->up);
}

glm::mat4 camera_t::projection ()
{
    return glm::perspective (
        glm::radians (m_d->fov), GS ()->width / GS ()->height, m_d->near, m_d->far);
}

void camera_t::drawDebug ()
{
    if (ImGui::Begin ("Camera"))
    {
        ImGui::SliderFloat ("FOV", &m_d->fov, 0.f, 180.f);
        ImGui::SliderFloat ("Near", &m_d->near, 0.1f, 1000.f);
        ImGui::SliderFloat ("Far", &m_d->far, 0.f, 10000.f);
        ImGui::InputFloat3 ("Eye", glm::value_ptr (m_d->eye));
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
