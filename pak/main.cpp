#include <cstdint>

struct pakInfo_t
{
    int32_t discSize;
    int32_t uncompressedSize;
    char compressionFlag;
    char info5;
    i;nt16_t offset;
};

int main (int argc, char *argv[])
{
    char bufferName[512];
    FILE *fileHandle;
    u32 fileOffset;

    strcpy (bufferName, homePath);
    strcat (bufferName, name); // temporary until makeExtention is coded
    strcat (bufferName, ".PAK");

    fileHandle = fopen (bufferName, "rb");

    if (!fileHandle)
        return 0;

    ASSERT (fileHandle);

    fseek (fileHandle, 4, SEEK_CUR);
    fread (&fileOffset, 4, 1, fileHandle);
    fread (&pPakInfo->discSize, 4, 1, fileHandle);
    fread (&pPakInfo->uncompressedSize, 4, 1, fileHandle);
    fread (&pPakInfo->compressionFlag, 1, 1, fileHandle);
    fread (&pPakInfo->info5, 1, 1, fileHandle);
    fread (&pPakInfo->offset, 2, 1, fileHandle);

    pPakInfo->discSize = READ_LE_U32 (&pPakInfo->discSize);
    pPakInfo->uncompressedSize = READ_LE_U32 (&pPakInfo->uncompressedSize);
    pPakInfo->offset = READ_LE_U16 (&pPakInfo->offset);


    return 0;
}
