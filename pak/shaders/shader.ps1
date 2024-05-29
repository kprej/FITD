../../bin/shaderc -p spirv  -i ../../build/_deps/bgfx-src/bgfx/src --varyingdef "$($args[0]).varying.def" --bin2c "$($args[0])_vs_spv" -o "$($args[0]).bin" --type v -f "$($args[0])_vs"

Get-Content "$($args[0]).bin" | Set-Content "$($args[0])_vs.h.bin"

../../bin/shaderc -p 440  -i ../../build/_deps/bgfx-src/bgfx/src --varyingdef "$($args[0]).varying.def" --bin2c "$($args[0])_vs_glsl" -o "$($args[0]).bin" --type v -f "$($args[0])_vs"

Get-Content "$($args[0]).bin" | Add-Content ".\$($args[0])_vs.h.bin"

"static const uint8_t $($args[0])_vs_essl[] = {0};" >> "$($args[0])_vs.h.bin"
"static const uint8_t $($args[0])_vs_dx11[] = {0};" >> "$($args[0])_vs.h.bin"

../../bin/shaderc -p spirv  -i ../../build/_deps/bgfx-src/bgfx/src --varyingdef "$($($args[0])).varying.def" --bin2c "$($($args[0]))_fs_spv" -o "$($($args[0])).bin" --type f -f "$($($args[0]))_fs"

Get-Content "$($args[0]).bin" | Set-Content "$($args[0])_fs.h.bin"

../../bin/shaderc -p 440  -i ../../build/_deps/bgfx-src//bgfx/src --varyingdef "$($args[0]).varying.def" --bin2c "$($args[0])_fs_glsl" -o "$($args[0]).bin" --type f -f "$($args[0])_fs"


Get-Content "$($args[0]).bin" | Add-Content ".\$($args[0])_fs.h.bin"

"static const uint8_t $($args[0])_fs_essl[] = {0};" >> "$($args[0])_fs.h.bin"
"static const uint8_t $($args[0])_fs_dx11[] = {0};" >> "$($args[0])_fs.h.bin"

rm "$($args[0]).bin"
