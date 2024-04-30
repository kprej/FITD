#include "bgfxShader.h"

#include <plog/Log.h>

#include <filesystem>
#include <fstream>
#include <iterator>
#include <vector>

using namespace std;

namespace
{

bgfx::ShaderHandle loadShader (std::string const &name_)
{
    PLOGD << "Loading shader: " << name_;
    filesystem::path const shader ("generated/" + name_ + ".bin");

    if (!filesystem::exists (shader))
    {
        PLOGD << "Can't open file: " << shader.string ();
        return {};
    }

    fstream file (shader, ios::in | ios::binary);

    if (!file.is_open ())
    {
        PLOGF << "Failed to load shader: " << shader.string ();
        return {};
    }

    vector<unsigned char> binary;
    copy (
        istream_iterator<char> (file), istream_iterator<char> (), back_inserter (binary));

    bgfx::Memory const *mem = bgfx::copy (binary.data (), binary.size ());
    bgfx::ShaderHandle handle = bgfx::createShader (mem);

    if (handle.idx > 512)
    {
        PLOGF << "Invalid shader handle";
        return {};
    }

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
