#include "bgfxHandle.h"
#include "bgfxShader.h"
#include "imgui_impl_bgfx.h"
#include "osystem.h"

#include <backends/imgui_impl_sdl3.h>
#include <bgfx/platform.h>
#include <imgui.h>

#include <plog/Log.h>

#include <array>
using namespace std;

class bgfxHandle_t::private_t
{
public:
    ~private_t () = default;
    private_t ()
        : initParam ()
        , backgroundTexture ()
        , paletteTexture ()
        , fieldModelInspectorFB (BGFX_INVALID_HANDLE)
        , fieldModelInspectorTexture (BGFX_INVALID_HANDLE)
        , fieldModelInspectorDepth (BGFX_INVALID_HANDLE)
        , backgroundShader (BGFX_INVALID_HANDLE)
        , maskBackgroundShader (BGFX_INVALID_HANDLE)
        , flatShader (BGFX_INVALID_HANDLE)
        , noiseShader (BGFX_INVALID_HANDLE)
        , rampShader (BGFX_INVALID_HANDLE)
        , oldWindowSize (-1, -1)
        , gameResolution (320, 200)
        , gameViewId (1)
        , backgroundMode (backgroundMode_t::_2D)
    {
    }

    bgfx::Init initParam;

    bgfx::TextureHandle backgroundTexture;
    bgfx::TextureHandle paletteTexture;

    // Debug Bits
    bgfx::FrameBufferHandle fieldModelInspectorFB;
    bgfx::TextureHandle fieldModelInspectorTexture;
    bgfx::TextureHandle fieldModelInspectorDepth;

    // Shader Handles
    bgfx::ProgramHandle backgroundShader;
    bgfx::ProgramHandle maskBackgroundShader;
    bgfx::ProgramHandle flatShader;
    bgfx::ProgramHandle noiseShader;
    bgfx::ProgramHandle rampShader;

    ImVec2 oldWindowSize;
    ImVec2 gameResolution;

    uint8_t gameViewId;
    backgroundMode_t backgroundMode;
};

bgfxHandle_t::~bgfxHandle_t () = default;

bgfxHandle_t::bgfxHandle_t ()
    : m_d (make_unique<private_t> ())
{
}

void bgfxHandle_t::init ()
{
    PLOGD << "Init BGFX";
#if SDL_PLATFORM_WINDOWS
    PLOGD << "Windows Platform";
    m_d->initParam.platformData.ndt = NULL;
    m_d->initParam.platformData.nwh =
        SDL_GetProperty (SDL_GetWindowProperties (GS ()->window),
                         SDL_PROP_WINDOW_WIN32_HWND_POINTER,
                         NULL);
#elif SDL_PLATFORM_LINUX
    PLOGD << "Linux Platform";
    if (SDL_strcmp (SDL_GetCurrentVideoDriver (), "x11") == 0)
    {
        m_d->initParam.platformData.ndt =
            SDL_GetProperty (SDL_GetWindowProperties (GS ()->window),
                             SDL_PROP_WINDOW_X11_DISPLAY_POINTER,
                             NULL);
        m_d->initParam.platformData.nwh = (void *)(uintptr_t)(SDL_GetNumberProperty (
            SDL_GetWindowProperties (GS ()->window),
            SDL_PROP_WINDOW_X11_WINDOW_NUMBER,
            0));
    }
    else if (SDL_strcmp (SDL_GetCurrentVideoDriver (), "wayland") == 0)
    {
        m_d->initParam.platformData.ndt =
            SDL_GetProperty (SDL_GetWindowProperties (GS ()->window),
                             SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER,
                             NULL);
        m_d->initParam.platformData.nwh =
            SDL_GetProperty (SDL_GetWindowProperties (GS ()->window),
                             SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER,
                             NULL);
    }
#endif

    m_d->initParam.type = bgfx::RendererType::OpenGL;
    m_d->initParam.resolution.width = 320;
    m_d->initParam.resolution.height = 200;
    m_d->initParam.resolution.reset = BGFX_RESET_VSYNC;
    if (!bgfx::init (m_d->initParam))
    {
        PLOGF << "Failed to init bgfx";
        return;
    }

    PLOGD << "Using renderer type: " << bgfx::getRendererName (bgfx::getRendererType ());

    PLOGD << "Init ImGui";
    if (!ImGui_ImplSDL3_InitForOpenGL (GS ()->window, nullptr))
    {
        PLOGF << "Failed to init ImGui";
        return;
    }

    PLOGD << "Create background Textures";
    m_d->backgroundTexture =
        bgfx::createTexture2D (320, 200, false, 1, bgfx::TextureFormat::R8U);
    m_d->paletteTexture =
        bgfx::createTexture2D (3, 256, false, 1, bgfx::TextureFormat::R8U);

    PLOGD << "Load shaders";
    m_d->backgroundShader = loadProgram ("background");
    m_d->maskBackgroundShader = loadProgram ("maskBackground");
    m_d->flatShader = loadProgram ("flat");
    m_d->noiseShader = loadProgram ("noise");
    m_d->rampShader = loadProgram ("ramp");
}

void bgfxHandle_t::startFrame ()
{
    int oldResolution[2];
    oldResolution[0] = GS ()->outputResolution[0];
    oldResolution[1] = GS ()->outputResolution[1];

    SDL_GetWindowSize (
        GS ()->window, &GS ()->outputResolution[0], &GS ()->outputResolution[1]);

    if ((oldResolution[0] != GS ()->outputResolution[0]) ||
        (oldResolution[1] != GS ()->outputResolution[1]))
    {
        bgfx::reset (GS ()->outputResolution[0], GS ()->outputResolution[1]);
    }

    /*
    ImGui_ImplSDL3_NewFrame ();

    // imguiBeginFrame (0, 0, 0, 0, outputResolution[0], outputResolution[1], -1);

    if (ImGui::BeginMainMenuBar ())
    {
        ImGui::Text (" %.2f FPS (%.2f ms)",
                     ImGui::GetIO ().Framerate,
                     1000.0f / ImGui::GetIO ().Framerate);

        ImGui::PushItemWidth (100);
        ImGui::SliderFloat ("Volume", &gVolume, 0, 1);
        ImGui::PopItemWidth ();

        ImGui::EndMainMenuBar ();
    }
    */

    bgfx::setViewRect (0, 0, 0, GS ()->outputResolution[0], GS ()->outputResolution[1]);
    bgfx::setViewClear (0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH);
    bgfx::touch (0);
    m_d->oldWindowSize = {-1, -1};

    if (GS ()->debugMenuDisplayed)
    {
        startDebugFrame ();
    }
    else
    {
        m_d->gameViewId = 0;
        m_d->gameResolution[0] = GS ()->outputResolution[0];
        m_d->gameResolution[1] = GS ()->outputResolution[1];
        bgfx::setViewFrameBuffer (m_d->gameViewId,
                                  BGFX_INVALID_HANDLE); // bind the backbuffer
    }

    bgfx::setViewClear (m_d->gameViewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 255);

    bgfx::setViewName (m_d->gameViewId, "Game");
    bgfx::setViewMode (m_d->gameViewId, bgfx::ViewMode::Sequential);

    bgfx::touch (m_d->gameViewId);

    drawBackground ();
}

void bgfxHandle_t::startDebugFrame ()
{
    m_d->gameViewId = 1;
    /*
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

    */
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
    bgfx::setViewFrameBuffer (m_d->gameViewId, m_d->fieldModelInspectorFB);
    bgfx::setViewRect (
        m_d->gameViewId, 0, 0, m_d->gameResolution[0], m_d->gameResolution[1]);
}

void bgfxHandle_t::drawBackground ()
{
    if (m_d->backgroundMode != backgroundMode_t::_2D)
        return;

    bgfx::VertexLayout layout;
    layout.begin ()
        .add (bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add (bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end ();

    bgfx::TransientVertexBuffer transientBuffer;
    bgfx::allocTransientVertexBuffer (&transientBuffer, 6, layout);

    struct sVertice
    {
        float position[3];
        float texcoord[2];
    };

    sVertice *pVertices = (sVertice *)transientBuffer.data;

    float quadVertices[6 * 3];
    float quadUV[6 * 2];

    // 0
    pVertices->position[0] = 0.f;
    pVertices->position[1] = 0.f;
    pVertices->position[2] = 1000.f;
    pVertices->texcoord[0] = 0.f;
    pVertices->texcoord[1] = 0.f;
    pVertices++;

    // 2
    pVertices->position[0] = 320.f;
    pVertices->position[1] = 200.f;
    pVertices->position[2] = 1000.f;
    pVertices->texcoord[0] = 1.f;
    pVertices->texcoord[1] = 1.f;
    pVertices++;

    // 1
    pVertices->position[0] = 320.f;
    pVertices->position[1] = 0.f;
    pVertices->position[2] = 1000.f;
    pVertices->texcoord[0] = 1.f;
    pVertices->texcoord[1] = 0.f;
    pVertices++;

    //------------------------
    // 3
    pVertices->position[0] = 0.f;
    pVertices->position[1] = 0.f;
    pVertices->position[2] = 1000.f;
    pVertices->texcoord[0] = 0.f;
    pVertices->texcoord[1] = 0.f;
    pVertices++;

    // 4
    pVertices->position[0] = 0.f;
    pVertices->position[1] = 200.f;
    pVertices->position[2] = 1000.f;
    pVertices->texcoord[0] = 0.f;
    pVertices->texcoord[1] = 1.f;
    pVertices++;

    // 5
    pVertices->position[0] = 320.f;
    pVertices->position[1] = 200.f;
    pVertices->position[2] = 1000.f;
    pVertices->texcoord[0] = 1.f;
    pVertices->texcoord[1] = 1.f;
    pVertices++;

    static bgfx::UniformHandle backgroundTextureUniform = BGFX_INVALID_HANDLE;
    if (!bgfx::isValid (backgroundTextureUniform))
    {
        backgroundTextureUniform =
            bgfx::createUniform ("s_backgroundTexture", bgfx::UniformType::Sampler);
    }
    static bgfx::UniformHandle paletteTextureUniform = BGFX_INVALID_HANDLE;
    if (!bgfx::isValid (paletteTextureUniform))
    {
        paletteTextureUniform =
            bgfx::createUniform ("s_paletteTexture", bgfx::UniformType::Sampler);
    }

    bgfx::setState (0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_MSAA);

    bgfx::setVertexBuffer (0, &transientBuffer);

    bgfx::setTexture (0, backgroundTextureUniform, m_d->backgroundTexture);
    bgfx::setTexture (1, paletteTextureUniform, m_d->paletteTexture);
    bgfx::submit (m_d->gameViewId, m_d->backgroundShader);
}
