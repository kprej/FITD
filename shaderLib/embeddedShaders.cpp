#include "embeddedShaders.h"

#include <bgfx/bgfx.h>

#include "generated/background_ps.bin"
#include "generated/background_vs.bin"
#include "generated/flat_ps.bin"
#include "generated/flat_vs.bin"
#include "generated/maskBackground_ps.bin"
#include "generated/maskBackground_vs.bin"
#include "generated/noise_ps.bin"
#include "generated/noise_vs.bin"
#include "generated/ramp_ps.bin"
#include "generated/ramp_vs.bin"

#include <cassert>

bgfx::ProgramHandle loadBgfxProgram (std::string const &file_)
{
    bgfx::RendererType::Enum type = bgfx::getRendererType ();

    bgfx::ProgramHandle ProgramHandle = bgfx::createProgram (
        bgfx::loadProgram ((file_ + "_vs").c_str (), (file_ + "_ps").c_str ()));

    assert (bgfx::isValid (ProgramHandle));

    return ProgramHandle;
}
