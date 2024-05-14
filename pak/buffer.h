#pragma once

#include <istream>
#include <vector>

class buf_t : public std::basic_streambuf<char>
{
public:
    buf_t (std::vector<std::byte> const &p_)
    {
        setg ((char *)p_.data (), (char *)p_.data (), (char *)p_.data () + p_.size ());
    }
};

class buffer_t : public std::istream
{
public:
    buffer_t (std::vector<std::byte> const &data_)
        : std::istream (&m_d)
        , m_d (data_)
    {
        rdbuf (&m_d);
    }

    template <typename T>
    T get ()
    {
        T type;
        this->read (reinterpret_cast<char *> (&type), sizeof (type));
        return type;
    }

private:
    buf_t m_d;
};
