#pragma once

#include <memory>

class introScene_t
{
public:
    ~introScene_t ();
    introScene_t ();

    bool run ();

private:
    void enter ();
    void infogram ();
    void dillo ();
    void exit ();

    void debug ();

    class private_t;
    std::shared_ptr<private_t> m_d;
};
