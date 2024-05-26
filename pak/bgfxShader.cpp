#include "bgfxShader.h"

#include "shaders/body_fs.h.bin"
#include "shaders/body_vs.h.bin"
#include "shaders/combine_fs.h.bin"
#include "shaders/combine_vs.h.bin"
#include "shaders/font_fs.h.bin"
#include "shaders/font_vs.h.bin"
#include "shaders/post_fs.h.bin"
#include "shaders/post_vs.h.bin"
#include "shaders/texture_fs.h.bin"
#include "shaders/texture_vs.h.bin"

#include <bgfx/embedded_shader.h>

#include <plog/Log.h>

using namespace std;

namespace
{
static const bgfx::EmbeddedShader s_embeddedShaders[] = {
    BGFX_EMBEDDED_SHADER (texture_fs),
    BGFX_EMBEDDED_SHADER (texture_vs),
    BGFX_EMBEDDED_SHADER (combine_fs),
    BGFX_EMBEDDED_SHADER (combine_vs),
    BGFX_EMBEDDED_SHADER (body_fs),
    BGFX_EMBEDDED_SHADER (body_vs),
    BGFX_EMBEDDED_SHADER (font_fs),
    BGFX_EMBEDDED_SHADER (font_vs),
    BGFX_EMBEDDED_SHADER (post_fs),
    BGFX_EMBEDDED_SHADER (post_vs),

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
