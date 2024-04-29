#include "bgfxShader.h"

#include <plog/Log.h>

#include <filesystem>
#include <fstream>

using namespace std;

namespace
{

bgfx::Memory const *loadMem (filesystem::path const &path_)
{
    fstream file (path_, ios::in | ios::binary);

    if (!file.is_open ())
    {
        PLOGF << "Failed to load shader: " << path_.string ();
        return nullptr;
    }

    uint32_t size = (uint32_t)file.tellg ();

    bgfx::Memory const *mem = bgfx::alloc (size + 1);

    file.read (reinterpret_cast<char *> (mem->data), size);
    file.close ();

    mem->data[mem->size - 1] = '\0';
    return mem;
}

bgfx::ShaderHandle loadShader (std::string const &name_)
{
    filesystem::path const shader ("generated/" + name_ + ".bin");

    if (!filesystem::exists (shader))
        return {};

    bgfx::ShaderHandle handle =
        bgfx::createShader (loadMem (filesystem::absolute (shader)));

    bgfx::setName (handle, name_.data (), name_.size ());

    return handle;
}
} // namespace

bgfx::ProgramHandle loadProgram (string const &name_)
{
    bgfx::ShaderHandle vsh = loadShader (name_ + "_vs");
    bgfx::ShaderHandle fsh = loadShader (name_ + "_ps");

    return bgfx::createProgram (
        vsh, fsh, true /* destroy shaders when program is destroyed */);
}
