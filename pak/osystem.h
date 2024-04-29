#pragma once
#include "types.h"

#include <SDL3/SDL.h>

#include <memory>
#include <vector>

enum class rendererType_t
{
    OPENGL_OLD,
    OPENGL_ES,
    OPENGL_3_2,
};

struct gameState_t
{
    gameId_t gameId;
    bool debugMenuDisplayed;
    std::vector<int16_t> CVars;
    int outputResolution[2];
    SDL_Window *window;
};

class osystem_t
{
public:
    ~osystem_t ();
    osystem_t ();

    static std::shared_ptr<gameState_t> GS ();

    void init (int argc, char *argv[]);

private:
    void detectGame ();

    class private_t;
    std::unique_ptr<private_t> m_d;
};

inline std::shared_ptr<gameState_t> GS ()
{
    return osystem_t::GS ();
}
