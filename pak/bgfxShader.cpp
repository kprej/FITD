#include "bgfxShader.h"

#include "shaders/background_fs.h.bin"
#include "shaders/background_vs.h.bin"
#include "shaders/combine_fs.h.bin"
#include "shaders/combine_vs.h.bin"
#include "shaders/flat_fs.h.bin"
#include "shaders/flat_vs.h.bin"
#include "shaders/font_fs.h.bin"
#include "shaders/font_vs.h.bin"
#include "shaders/maskBackground_fs.h.bin"
#include "shaders/maskBackground_vs.h.bin"
#include "shaders/noise_fs.h.bin"
#include "shaders/noise_vs.h.bin"
#include "shaders/post_fs.h.bin"
#include "shaders/post_vs.h.bin"
#include "shaders/ramp_fs.h.bin"
#include "shaders/ramp_vs.h.bin"

#include <bgfx/embedded_shader.h>

#include <plog/Log.h>

using namespace std;

namespace
{
static const bgfx::EmbeddedShader s_embeddedShaders[] = {
    BGFX_EMBEDDED_SHADER (background_fs),
    BGFX_EMBEDDED_SHADER (background_vs),
    BGFX_EMBEDDED_SHADER (combine_fs),
    BGFX_EMBEDDED_SHADER (combine_vs),
    BGFX_EMBEDDED_SHADER (flat_fs),
    BGFX_EMBEDDED_SHADER (flat_vs),
    BGFX_EMBEDDED_SHADER (font_fs),
    BGFX_EMBEDDED_SHADER (font_vs),
    BGFX_EMBEDDED_SHADER (maskBackground_fs),
    BGFX_EMBEDDED_SHADER (maskBackground_vs),
    BGFX_EMBEDDED_SHADER (noise_fs),
    BGFX_EMBEDDED_SHADER (noise_vs),
    BGFX_EMBEDDED_SHADER (post_fs),
    BGFX_EMBEDDED_SHADER (post_vs),
    BGFX_EMBEDDED_SHADER (ramp_fs),
    BGFX_EMBEDDED_SHADER (ramp_vs),

    BGFX_EMBEDDED_SHADER_END ()};

} // namespace

bgfx::ProgramHandle loadProgram (string const &name_)
{
    bgfx::RendererType::Enum type = bgfx::getRendererType ();

    bgfx::ProgramHandle ProgramHandle = bgfx::createProgram (
        bgfx::createEmbeddedShader (s_embeddedShaders, type, (name_ + "_vs").c_str ()),
        bgfx::createEmbeddedShader (s_embeddedShaders, type, (name_ + "_fs").c_str ()),
        true);

    assert (bgfx::isValid (ProgramHandle));

    PLOGD << "Loaded shader: " << name_ << " " << ProgramHandle.idx;

    return ProgramHandle;
}
