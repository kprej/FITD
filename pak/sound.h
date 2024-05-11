#pragma once

#include <memory>
#include <vector>

class sound_t
{
public:
    ~sound_t ();
    sound_t ();

    void init (std::vector<std::byte> const &data_);

    void play ();

private:
    class private_t;
    std::shared_ptr<private_t> m_d;
};
