#pragma once

#include "pakFile.h"

#include <memory>

class font_t
{
public:
    ~font_t ();
    font_t ();

    void init (pak_t const &pak_);

private:
    class private_t;
    std::shared_ptr<private_t> m_d;
};
