#pragma once

#include <glm/glm.hpp>

#include <memory>

class camera_t
{
public:
    ~camera_t ();
    camera_t ();

    void init ();

    void setViewPos (glm::vec2 const &vec_);
    void setViewSize (glm::vec2 const &vec_);

    void setFOV (float const &fov_);
    void setPos (glm::vec3 const &vec_);
    void lookAt (glm::vec3 const &vec_);

    glm::vec2 viewPos () const;
    glm::vec2 viewSize () const;

    glm::mat4 view ();
    glm::mat4 projection ();

protected:
    friend class debugHandle_t;
    void drawDebug ();

private:
    class private_t;
    std::unique_ptr<private_t> m_d;
};
