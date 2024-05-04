#pragma once

#include <cstdint>
#include <fstream>
#include <memory>
#include <vector>

class pakInfo_t
{
public:
    ~pakInfo_t ();
    pakInfo_t ();

    void init (std::fstream &file_, uint8_t index_);

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
    pak_t (std::string const &file_);

    void init ();

    std::vector<std::byte> const &data (uint8_t index_) const;

private:
    std::string m_file;
    std::vector<pakInfo_t> m_paks;
};
