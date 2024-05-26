#include "debugHandle.h"
#include "imgui_impl_bgfx.h"
#include "osystem.h"

#include "AITD/samples.h"

#include <backends/imgui_impl_sdl3.h>
#include <bgfx/platform.h>

#include <imgui.h>

#include <magic_enum.hpp>
#include <plog/Log.h>

#include <array>
using namespace std;

class debugHandle_t::private_t
{
public:
    ~private_t () {}

    private_t ()
        : init (false)
        , viewId (0)
        , frameBuffer (BGFX_INVALID_HANDLE)
    {
    }

    bool init;
    uint8_t viewId;
    bgfx::FrameBufferHandle frameBuffer;
};

debugHandle_t::~debugHandle_t ()
{
}

debugHandle_t::debugHandle_t ()
    : m_d (make_unique<private_t> ())
{
}

void debugHandle_t::init (SDL_Window *window_)
{
    PLOGD << "Init ImGui";
    ImGui::CreateContext ();

    ImGui_Implbgfx_Init (m_d->viewId);
    if (!ImGui_ImplSDL3_InitForVulkan (window_))
    {
        PLOGF << "Failed to init ImGui";
        return;
    }

    m_d->init = true;
}

void debugHandle_t::startFrame ()
{
    ImGui_Implbgfx_NewFrame ();
    ImGui_ImplSDL3_NewFrame ();

    ImGui::NewFrame ();

    if (ImGui::BeginMainMenuBar ())
    {
        if (ImGui::BeginMenu ("Windows"))
        {
            showSampleWindow ();
            ImGui::EndMenu ();
        }
        ImGui::Text (" %.2f FPS (%.2f ms)",
                     ImGui::GetIO ().Framerate,
                     1000.0f / ImGui::GetIO ().Framerate);

        ImGui::EndMainMenuBar ();
    }

    if (GS ()->screenSizeChanged || !bgfx::isValid (m_d->frameBuffer))
    {
        PLOGD << "Create debug frame buffer";

        m_d->frameBuffer = bgfx::createFrameBuffer (bgfx::BackbufferRatio::Equal,
                                                    bgfx::TextureFormat::BGRA8);

        bgfx::setViewFrameBuffer (m_d->viewId, m_d->frameBuffer);
    }

    bgfx::setViewClear (m_d->viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0, 1.0f, 0);

    bgfx::touch (m_d->viewId);

    draw.fire ();
}

void debugHandle_t::endFrame ()
{
    ImGui::Render ();

    ImGui_Implbgfx_RenderDrawLists (ImGui::GetDrawData ());
}

void debugHandle_t::shutdown ()
{
    if (!m_d->init)
        return;

    PLOGD << "Shutdown ImGui";
    ImGui_ImplSDL3_Shutdown ();
    ImGui_Implbgfx_Shutdown ();

    if (bgfx::isValid (m_d->frameBuffer))
        bgfx::destroy (m_d->frameBuffer);

    ImGui::DestroyContext ();
}

void debugHandle_t::showSampleWindow ()
{
    if (ImGui::Begin ("Samples"))
    {
        for (auto i = 0; i < GS ()->samples.size (); ++i)
        {
            switch (GS ()->gameId)
            {
            case gameId_t::AITD1:

                if (ImGui::Button (magic_enum::enum_name (
                                       magic_enum::enum_value<atid::samples_t> (i))
                                       .data ()))
                    GS ()->samples.at (i).play ();
                break;
            };
        }
    }
    ImGui::End ();
}

bgfx::TextureHandle debugHandle_t::texture ()
{
    return bgfx::getTexture (m_d->frameBuffer);
}
