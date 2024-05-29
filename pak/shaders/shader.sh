#! /bin/bash
../../bin/shaderc -p spirv  -i ~/dev/bgfx/src --varyingdef $1.varying.def --bin2c $1_vs_spv -o $1.bin --type v -f $1_vs
cat $1.bin > $1_vs.h.bin

../../bin/tools/bin/linux/shaderc -p 440  -i ~/dev/bgfx/src --varyingdef $1.varying.def --bin2c $1_vs_glsl -o $1.bin --type v -f $1_vs
cat $1.bin >> $1_vs.h.bin

echo "static const uint8_t $1_vs_essl[] = {0};" | cat >> $1_vs.h.bin
echo "static const uint8_t $1_vs_dx11[] = {0};" | cat >> $1_vs.h.bin

../../bin/linux/shaderc -p spirv  -i ~/dev/bgfx/src --varyingdef $1.varying.def --bin2c $1_fs_spv -o $1.bin --type f -f $1_fs
cat $1.bin > $1_fs.h.bin

../../bin/linux/shaderc -p 440  -i ~/dev/bgfx/src --varyingdef $1.varying.def --bin2c $1_fs_glsl -o $1.bin --type f -f $1_fs
cat $1.bin >> $1_fs.h.bin

echo "static const uint8_t $1_fs_essl[] = {0};" | cat >> $1_fs.h.bin
echo "static const uint8_t $1_fs_dx11[] = {0};" | cat >> $1_fs.h.bin

rm $1.bin
