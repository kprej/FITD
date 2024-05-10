#pragma once

class scene_t
{
public:
    virtual ~scene_t () = 0;
    scene_t ();

    virtual void run () = 0;
};
