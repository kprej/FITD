#pragma once

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <memory>
#include <vector>

class pakFile_t;
enum class pakType_t
{
    PALETTE,
    TEXTURE,
    PALETTE_TEXTURE,
    BODY,
    SOUND,
    TEXT,
};

class pak_t
{
public:
    ~pak_t ();
    pak_t ();

    std::vector<std::byte> const &data () const;

protected:
    friend class pakFile_t;

    void init (std::fstream &file_);

private:
    void unpak (std::fstream &file_);
    void sanitizeName ();

    class private_t;

    std::shared_ptr<private_t> m_d;
};

class pakFile_t
{
public:
    ~pakFile_t ();
    pakFile_t (std::filesystem::path const &file_);

    pak_t const &pak (uint8_t index_) const;

    std::vector<std::byte> const &data (uint8_t index_) const;
    std::vector<pak_t> const &paks () const;

private:
    class private_t;
    std::shared_ptr<private_t> m_d;
};
