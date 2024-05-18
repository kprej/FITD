#pragma once

#include "pakFile.h"

#include <glm/glm.hpp>

#include <memory>

class font_t
{
public:
    ~font_t ();
    font_t ();

    void init (pak_t const &pak_, uint16_t textureWidth_);

    void render (glm::vec2 const &pos_, std::string const &text_);

private:
    class private_t;
    std::shared_ptr<private_t> m_d;
};
