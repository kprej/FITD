#include "bgfxHandle.h"
#include "bgfxShader.h"
#include "imgui_impl_bgfx.h"
#include "osystem.h"

#include <backends/imgui_impl_sdl3.h>
#include <bgfx/platform.h>
#include <bx/math.h>
#include <imgui.h>
#include <plog/Helpers/HexDump.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <plog/Log.h>

#include <array>
using namespace std;

class bgfxHandle_t::private_t
{
public:
    ~private_t () {}

    private_t ()
        : initParam ()
        , backgroundTexture (BGFX_INVALID_HANDLE)
        , paletteTexture (BGFX_INVALID_HANDLE)
        , backgroundTextureUniform (BGFX_INVALID_HANDLE)
        , paletteTextureUniform (BGFX_INVALID_HANDLE)
        , alphaTextureUniform (BGFX_INVALID_HANDLE)
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
        , gameViewId (0)
        , debugViewId (2)
        , backgroundMode (backgroundMode_t::_2D)
        , alpha (0)
    {
    }

    bgfx::Init initParam;

    bgfx::TextureHandle backgroundTexture;
    bgfx::TextureHandle paletteTexture;

    bgfx::UniformHandle backgroundTextureUniform;
    bgfx::UniformHandle paletteTextureUniform;
    bgfx::UniformHandle alphaTextureUniform;
    bgfx::UniformHandle polyColorUniform;

    bgfx::VertexLayout backgroundLayout;
    bgfx::VertexLayout bodyLayout;

    backgroundState_t backgroundState;
    float fadeStep;
    int fadeTimeMSec;

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
    uint8_t debugViewId;
    backgroundMode_t backgroundMode;

    array<byte, 64000> physicalScreen;
    array<byte, 192000> physicalScreenRGB;

    int outputResolution[2];
    SDL_Window *window;
    array<byte, 768> currentPalette;
    float alpha;
};

bgfxHandle_t::~bgfxHandle_t ()
{
}

bgfxHandle_t::bgfxHandle_t ()
    : m_d (make_unique<private_t> ())
{
}

void bgfxHandle_t::init ()
{
    PLOGD << "Create window";
    m_d->window = SDL_CreateWindow ("FITD",
                                    1280,
                                    800,
                                    SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY |
                                        SDL_WINDOW_HIDDEN);

    if (!m_d->window)
    {
        PLOGF << SDL_GetError ();
        return;
    }

    PLOGD << "Init BGFX";
#if SDL_PLATFORM_WINDOWS
    PLOGD << "Windows Platform";
    m_d->initParam.platformData.ndt = NULL;
    m_d->initParam.platformData.nwh = SDL_GetProperty (
        SDL_GetWindowProperties (m_d->window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
#elif SDL_PLATFORM_LINUX
    PLOGD << "Linux Platform";
    if (SDL_strcmp (SDL_GetCurrentVideoDriver (), "x11") == 0)
    {
        m_d->initParam.platformData.ndt =
            SDL_GetProperty (SDL_GetWindowProperties (m_d->window),
                             SDL_PROP_WINDOW_X11_DISPLAY_POINTER,
                             NULL);
        m_d->initParam.platformData.nwh = (void *)(uintptr_t)(SDL_GetNumberProperty (
            SDL_GetWindowProperties (m_d->window), SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0));
    }
    else if (SDL_strcmp (SDL_GetCurrentVideoDriver (), "wayland") == 0)
    {
        m_d->initParam.platformData.ndt =
            SDL_GetProperty (SDL_GetWindowProperties (m_d->window),
                             SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER,
                             NULL);
        m_d->initParam.platformData.nwh =
            SDL_GetProperty (SDL_GetWindowProperties (m_d->window),
                             SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER,
                             NULL);
    }
#endif

    m_d->initParam.type = bgfx::RendererType::Vulkan;
    m_d->initParam.resolution.width = 320;
    m_d->initParam.resolution.height = 200;
    m_d->initParam.resolution.reset = BGFX_RESET_VSYNC;

    auto caps = bgfx::getCaps ();

    if (!bgfx::init (m_d->initParam))
    {
        PLOGF << "Failed to init bgfx";
        return;
    }

    PLOGD << "Using renderer type: " << bgfx::getRendererName (bgfx::getRendererType ());

    PLOGD << "Init ImGui";
    ImGui::CreateContext ();

    ImGui_Implbgfx_Init (255);
    if (!ImGui_ImplSDL3_InitForOpenGL (m_d->window, nullptr))
    {
        PLOGF << "Failed to init ImGui";
        return;
    }

    m_d->backgroundLayout.begin ()
        .add (bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add (bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end ();

    m_d->bodyLayout.begin ()
        .add (bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .end ();

    PLOGD << "Create background Textures";
    m_d->backgroundTexture =
        bgfx::createTexture2D (320, 200, false, 1, bgfx::TextureFormat::R8U);
    m_d->paletteTexture =
        bgfx::createTexture2D (3, 256, false, 1, bgfx::TextureFormat::R8U);

    m_d->backgroundTextureUniform =
        bgfx::createUniform ("s_backgroundTexture", bgfx::UniformType::Sampler);
    m_d->paletteTextureUniform =
        bgfx::createUniform ("s_paletteTexture", bgfx::UniformType::Sampler);

    m_d->alphaTextureUniform = bgfx::createUniform ("s_alpha", bgfx::UniformType::Vec4);
    m_d->polyColorUniform = bgfx::createUniform ("s_polyColor", bgfx::UniformType::Vec4);

    PLOGD << "Load shaders";
    m_d->backgroundShader = loadProgram ("background");
    m_d->flatShader = loadProgram ("flat");
}

void bgfxHandle_t::startFrame ()
{
    if (SDL_GetWindowFlags (m_d->window) & SDL_WINDOW_HIDDEN)
    {
        SDL_SetWindowTitle (m_d->window, toString (GS ()->gameId).c_str ());
    }

    SDL_ShowWindow (m_d->window);
    int oldResolution[2];
    oldResolution[0] = m_d->outputResolution[0];
    oldResolution[1] = m_d->outputResolution[1];

    SDL_GetWindowSize (m_d->window, &m_d->outputResolution[0], &m_d->outputResolution[1]);

    if ((oldResolution[0] != m_d->outputResolution[0]) ||
        (oldResolution[1] != m_d->outputResolution[1]))
    {
        bgfx::reset (
            m_d->outputResolution[0], m_d->outputResolution[1], BGFX_RESET_HIDPI);
    }

    ImGui_Implbgfx_NewFrame ();
    ImGui_ImplSDL3_NewFrame ();

    ImGui::NewFrame ();

    bgfx::setViewRect (0, 0, 0, m_d->outputResolution[0], m_d->outputResolution[1]);
    bgfx::setViewClear (0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 255);
    bgfx::touch (0);

    m_d->oldWindowSize = {-1, -1};

    if (GS ()->debugMenuDisplayed)
    {
        startDebugFrame ();
    }

    m_d->gameResolution[0] = m_d->outputResolution[0];
    m_d->gameResolution[1] = m_d->outputResolution[1];

    bgfx::setViewFrameBuffer (m_d->gameViewId,
                              BGFX_INVALID_HANDLE); // bind the backbuffer

    bgfx::setViewClear (
        m_d->gameViewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 255, 1.0f, 0);

    bgfx::setViewClear (
        m_d->debugViewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 255, 1.0f, 0);

    bgfx::setViewName (m_d->gameViewId, "Game");
    bgfx::setViewName (m_d->debugViewId, "Debug");
    bgfx::setViewMode (m_d->gameViewId, bgfx::ViewMode::Sequential);
    const glm::vec3 at = {0.0f, 0.0f, 0.0f};
    const glm::vec3 eye = {0.0f, 0.0f, -65.0f};
    const glm::vec3 up = {0.0f, 1.0f, 0.0f};

    auto view = glm::lookAt (eye, at, up);

    const bgfx::Caps *caps = bgfx::getCaps ();

    auto const proj =
        glm::perspective (glm::radians (60.f),
                          float (m_d->gameResolution[0]) / float (m_d->gameResolution[1]),
                          0.1f,
                          1000.0f);

    // Set view and projection matrix for view 0.
    bgfx::setViewTransform (
        m_d->gameViewId, glm::value_ptr (view), glm::value_ptr (proj));

    bgfx::touch (m_d->gameViewId);

    drawBackground ();
}

void bgfxHandle_t::endFrame ()
{
    ImGui::Render ();

    ImGui_Implbgfx_RenderDrawLists (ImGui::GetDrawData ());

    bgfx::frame ();
}

void bgfxHandle_t::setPalette (vector<byte> const &palette_)
{
    bgfx::updateTexture2D (
        m_d->paletteTexture, 0, 0, 0, 0, 3, 256, bgfx::copy (palette_.data (), 256 * 3));
}

void bgfxHandle_t::setBackground (vector<byte> const &texture_, int offset_)
{
    bgfx::updateTexture2D (m_d->backgroundTexture,
                           0,
                           0,
                           0,
                           0,
                           320,
                           200,
                           bgfx::copy (texture_.data () + offset_, 320 * 200));
}

void bgfxHandle_t::fadeInBackground (int msec_)
{
    if (m_d->backgroundState == backgroundState_t::FADING_IN)
        return;

    m_d->backgroundState = backgroundState_t::FADING_IN;
    m_d->fadeStep = 0;
    m_d->fadeTimeMSec = msec_;
}

void bgfxHandle_t::fadeOutBackground (int msec_)
{
    if (m_d->backgroundState == backgroundState_t::FADING_OUT)
        return;

    m_d->backgroundState = backgroundState_t::FADING_OUT;
    m_d->fadeStep = 255;
    m_d->fadeTimeMSec = msec_;
}

backgroundState_t bgfxHandle_t::backgroundState () const
{
    return m_d->backgroundState;
}

void bgfxHandle_t::drawBody (body_t const &body_)
{
    for (auto const &p : body_.primitives ())
    {
        bgfx::setTransform (glm::value_ptr (body_.transform ()));

        bgfx::setVertexBuffer (0, body_.vertexBuffer ());
        bgfx::setIndexBuffer (body_.indexBuffer (), p.start, p.size);

        bgfx::setTexture (1, m_d->paletteTextureUniform, m_d->paletteTexture);
        bgfx::setUniform (m_d->polyColorUniform, &p.color);

        unsigned long state = 0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
                              BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS |
                              BGFX_STATE_CULL_CCW | BGFX_STATE_MSAA;

        if (p.size == 2)
            state |= BGFX_STATE_PT_LINES;

        bgfx::setState (state);
        bgfx::submit (m_d->gameViewId, m_d->flatShader);
    }
}

bgfx::VertexLayout const &bgfxHandle_t::bodyVertexLayout () const
{
    return m_d->bodyLayout;
}

void bgfxHandle_t::shutdown ()
{
    PLOGD << "Destroy shaders";

    bgfx::destroy (m_d->backgroundShader);
    bgfx::destroy (m_d->flatShader);

    bgfx::destroy (m_d->backgroundTexture);
    bgfx::destroy (m_d->paletteTexture);

    bgfx::destroy (m_d->backgroundTextureUniform);
    bgfx::destroy (m_d->paletteTextureUniform);
    bgfx::destroy (m_d->alphaTextureUniform);
    bgfx::destroy (m_d->polyColorUniform);

    PLOGD << "Shutdown ImGui";
    ImGui_ImplSDL3_Shutdown ();
    ImGui_Implbgfx_Shutdown ();

    ImGui::DestroyContext ();

    PLOGD << "Shutdown BGFX";
    bgfx::shutdown ();

    SDL_DestroyWindow (m_d->window);
}

void bgfxHandle_t::startDebugFrame ()
{
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

void bgfxHandle_t::drawBackground ()
{
    if (m_d->backgroundMode != backgroundMode_t::_2D)
        return;

    switch (m_d->backgroundState)
    {
    case backgroundState_t::FADING_IN:
        processFadeIn ();
        break;
    case backgroundState_t::FADING_OUT:
        processFadeOut ();
        break;
    case backgroundState_t::VISIBLE:
    case backgroundState_t::INVISIBLE:
        break;
    };

    bgfx::TransientVertexBuffer transientBuffer;
    bgfx::allocTransientVertexBuffer (&transientBuffer, 6, m_d->backgroundLayout);

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

    bgfx::setVertexBuffer (0, &transientBuffer);

    bgfx::setState (0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
                    BGFX_STATE_BLEND_SRC_ALPHA);

    bgfx::setTexture (0, m_d->backgroundTextureUniform, m_d->backgroundTexture);
    bgfx::setTexture (1, m_d->paletteTextureUniform, m_d->paletteTexture);

    bgfx::setUniform (m_d->alphaTextureUniform, &m_d->alpha);

    bgfx::submit (m_d->gameViewId, m_d->backgroundShader);
}

void bgfxHandle_t::processFadeIn ()
{
    m_d->fadeStep += GS ()->delta;
    m_d->alpha = glm::smoothstep (0.f, 1.f, m_d->fadeStep / m_d->fadeTimeMSec);

    if (m_d->alpha < 1.f)
        m_d->backgroundState = backgroundState_t::FADING_IN;

    else if (m_d->alpha == 1)
        m_d->backgroundState = backgroundState_t::VISIBLE;
}

void bgfxHandle_t::processFadeOut ()
{
    m_d->fadeStep -= GS ()->delta;
    m_d->alpha = glm::smoothstep (0.f, 1.f, m_d->fadeStep / m_d->fadeTimeMSec);
    if (m_d->alpha > 0.f)
        m_d->backgroundState = backgroundState_t::FADING_OUT;

    else if (m_d->alpha == 0)
        m_d->backgroundState = backgroundState_t::INVISIBLE;
}

bool bgfxHandle_t::windowHidden () const
{
    return (SDL_GetWindowFlags (m_d->window) & SDL_WINDOW_HIDDEN);
}
