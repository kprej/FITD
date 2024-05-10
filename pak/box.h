#pragma once

#include <vector>

class box_t
{
public:
    ~box_t ();
    box_t ();

    void setTexture (std::vector<std::byte> const &data_);
};
