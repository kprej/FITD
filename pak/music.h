#pragma once

#include <memory>
#include <vector>

class music_t
{
public:
    ~music_t ();
    music_t ();

    void init (std::vector<std::byte> const &data_);

    void play () const;

private:
    class private_t;
    std::shared_ptr<private_t> m_d;
};
