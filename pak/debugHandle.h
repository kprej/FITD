#pragma once

#include <SDL3/SDL.h>
#include <nano_signal_slot.hpp>

#include <memory>

class bgfxHandle_t;
class debugHandle_t
{
public:
    ~debugHandle_t ();
    debugHandle_t ();

    /// \name Signals
    /// @{
    Nano::Signal<void ()> draw;
    /// @}

protected:
    friend class bgfxHandle_t;
    void init (SDL_Window *window_);
    void startFrame ();
    void endFrame ();
    void shutdown ();

private:
    class private_t;
    std::unique_ptr<private_t> m_d;
};
