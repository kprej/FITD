#include "debugHandle.h"
#include "imgui_impl_bgfx.h"

#include <backends/imgui_impl_sdl3.h>
#include <bgfx/platform.h>

#include <imgui.h>

#include <plog/Log.h>

#include <array>
using namespace std;

class debugHandle_t::private_t
{
public:
    ~private_t () {}

    private_t ()
        : fieldModelInspectorFB (BGFX_INVALID_HANDLE)
        , fieldModelInspectorTexture (BGFX_INVALID_HANDLE)
        , fieldModelInspectorDepth (BGFX_INVALID_HANDLE)
        , oldWindowSize (-1, -1)
        , gameResolution (320, 200)
        , debugViewId (2)
    {
    }

    bgfx::FrameBufferHandle fieldModelInspectorFB;
    bgfx::TextureHandle fieldModelInspectorTexture;
    bgfx::TextureHandle fieldModelInspectorDepth;

    ImVec2 oldWindowSize;
    ImVec2 gameResolution;

    uint8_t debugViewId;
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

    ImGui_Implbgfx_Init (255);
    if (!ImGui_ImplSDL3_InitForVulkan (window_))
    {
        PLOGF << "Failed to init ImGui";
        return;
    }
}

void debugHandle_t::startFrame ()
{
    ImGui_Implbgfx_NewFrame ();
    ImGui_ImplSDL3_NewFrame ();

    ImGui::NewFrame ();

    bgfx::setViewName (m_d->debugViewId, "Debug");

    if (ImGui::BeginMainMenuBar ())
    {
        ImGui::Text (" %.2f FPS (%.2f ms)",
                     ImGui::GetIO ().Framerate,
                     1000.0f / ImGui::GetIO ().Framerate);

        ImGui::EndMainMenuBar ();
    }

    if (ImGui::Begin ("Game"))
    {
        ImVec2 currentWindowSize = ImGui::GetContentRegionAvail ();

        currentWindowSize[0] = std::max<int> (currentWindowSize[0], 1);
        currentWindowSize[1] = std::max<int> (currentWindowSize[1], 1);

        m_d->gameResolution = currentWindowSize;
    }
    else
    {
        m_d->gameResolution = {320, 200};
    }
    ImGui::End ();

    if ((m_d->gameResolution[0] != m_d->oldWindowSize[0]) ||
        (m_d->gameResolution[1] != m_d->oldWindowSize[1]))
    {
        m_d->oldWindowSize = m_d->gameResolution;

        if (bgfx::isValid (m_d->fieldModelInspectorFB))
        {
            bgfx::destroy (m_d->fieldModelInspectorFB);
        }

        const uint64_t tsFlags = 0
                                 //| BGFX_SAMPLER_MIN_POINT
                                 //| BGFX_SAMPLER_MAG_POINT
                                 //| BGFX_SAMPLER_MIP_POINT
                                 | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;

        m_d->fieldModelInspectorTexture =
            bgfx::createTexture2D (m_d->gameResolution[0],
                                   m_d->gameResolution[1],
                                   false,
                                   0,
                                   bgfx::TextureFormat::BGRA8,
                                   BGFX_TEXTURE_RT | tsFlags);
        m_d->fieldModelInspectorDepth = bgfx::createTexture2D (m_d->gameResolution[0],
                                                               m_d->gameResolution[1],
                                                               false,
                                                               0,
                                                               bgfx::TextureFormat::D24S8,
                                                               BGFX_TEXTURE_RT | tsFlags);
        array<bgfx::Attachment, 2> attachements;
        attachements[0].init (m_d->fieldModelInspectorTexture);
        attachements[1].init (m_d->fieldModelInspectorDepth);
        m_d->fieldModelInspectorFB = bgfx::createFrameBuffer (2, &attachements[0], true);
    }

    bgfx::setViewFrameBuffer (m_d->debugViewId, m_d->fieldModelInspectorFB);
    bgfx::setViewRect (
        m_d->debugViewId, 0, 0, m_d->gameResolution[0], m_d->gameResolution[1]);
}

void debugHandle_t::endFrame ()
{
    ImGui::Render ();

    ImGui_Implbgfx_RenderDrawLists (ImGui::GetDrawData ());
}

void debugHandle_t::shutdown ()
{
    PLOGD << "Shutdown ImGui";
    ImGui_ImplSDL3_Shutdown ();
    ImGui_Implbgfx_Shutdown ();

    ImGui::DestroyContext ();
}
