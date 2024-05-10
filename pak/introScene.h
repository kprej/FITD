#pragma once

#include "scene.h"

#include <memory>

class introScene_t
{
public:
    ~introScene_t ();
    introScene_t ();

    void enter ();
    void run ();
    void exit ();
    void debug ();

private:
    class private_t;
    std::shared_ptr<private_t> m_d;
};
