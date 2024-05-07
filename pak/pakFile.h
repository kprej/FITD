#pragma once

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <memory>
#include <vector>

class pakInfo_t
{
public:
    ~pakInfo_t ();
    pakInfo_t ();

    void init (std::fstream &file_);

    std::vector<std::byte> const &data () const;

private:
    void unpak (std::fstream &file_);
    void sanitizeName ();

    class private_t;

    std::shared_ptr<private_t> m_d;
};

class pak_t
{
public:
    ~pak_t ();
    pak_t (std::filesystem::path const &file_);

    std::vector<std::byte> const &data (uint8_t index_) const;

private:
    class private_t;
    std::shared_ptr<private_t> m_d;
};
