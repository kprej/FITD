#pragma once

#include <memory>

class titleScene_t
{
public:
    ~titleScene_t ();
    titleScene_t ();

    bool run ();

private:
    void enter ();
    void title ();
    void dillo ();
    void exit ();

    void debug ();

    class private_t;
    std::shared_ptr<private_t> m_d;
};
