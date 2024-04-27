#include "embeddedShaders.h"
#include "embeddedShadersMacro.h"

#include "generated/background_ps_440.bin"
#include "generated/background_vs_440.bin"
#include "generated/flat_ps_440.bin"
#include "generated/flat_vs_440.bin"
#include "generated/maskBackground_ps_440.bin"
#include "generated/maskBackground_vs_440.bin"
#include "generated/noise_ps_440.bin"
#include "generated/noise_vs_440.bin"
#include "generated/ramp_ps_440.bin"
#include "generated/ramp_vs_440.bin"

#include <cassert>

static const bgfx::EmbeddedShader s_embeddedShaders[] = {
    BGFX_EMBEDDED_SHADER (background_vs),
    BGFX_EMBEDDED_SHADER (background_ps),
    BGFX_EMBEDDED_SHADER (maskBackground_vs),
    BGFX_EMBEDDED_SHADER (maskBackground_ps),
    BGFX_EMBEDDED_SHADER (flat_vs),
    BGFX_EMBEDDED_SHADER (flat_ps),
    BGFX_EMBEDDED_SHADER (noise_vs),
    BGFX_EMBEDDED_SHADER (noise_ps),
    BGFX_EMBEDDED_SHADER (ramp_vs),
    BGFX_EMBEDDED_SHADER (ramp_ps),

    BGFX_EMBEDDED_SHADER_END ()};

bgfx::ProgramHandle loadBgfxProgram (std::string const &file_)
{
    bgfx::RendererType::Enum type = bgfx::getRendererType ();

    bgfx::ProgramHandle ProgramHandle = bgfx::createProgram (
        bgfx::createEmbeddedShader (s_embeddedShaders, type, (file_ + "_vs").c_str ()),
        bgfx::createEmbeddedShader (s_embeddedShaders, type, (file_ + "_ps").c_str ()),
        true);

    assert (bgfx::isValid (ProgramHandle));

    return ProgramHandle;
}
