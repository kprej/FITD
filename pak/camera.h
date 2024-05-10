#pragma once

#include <glm/glm.hpp>

#include <memory>

class camera_t
{
public:
    ~camera_t ();
    camera_t ();

    void init ();

    glm::mat4 view ();
    glm::mat4 projection ();

protected:
    friend class debugHandle_t;
    void drawDebug ();

private:
    class private_t;
    std::unique_ptr<private_t> m_d;
};
