#include "pakFile.h"
#include "unpack.h"

#include <plog/Formatters/TxtFormatter.h>
#include <plog/Helpers/HexDump.h>
#include <plog/Log.h>

#include <fstream>
#include <list>
#include <string>
using namespace std;

class pakInfo_t::private_t
{
public:
    ~private_t () = default;
    private_t ()
        : fileOffset ()
        , diskSize ()
        , uncompressedSize ()
        , compressionFlag ()
        , info5 ()
        , offset ()
        , name ()
        , data ()
    {
    }

    uint8_t index;
    int32_t fileOffset;
    int32_t diskSize;
    int32_t uncompressedSize;
    char compressionFlag;
    char info5;
    uint16_t offset;
    string name;
    vector<byte> data;
};

pakInfo_t::~pakInfo_t () = default;
pakInfo_t::pakInfo_t ()
    : m_d (make_shared<private_t> ())
{
}

void pakInfo_t::init (fstream &file_)
{
    auto initPos = file_.tellg ();

    file_.read (reinterpret_cast<char *> (&m_d->diskSize), 4);
    file_.read (reinterpret_cast<char *> (&m_d->uncompressedSize), 4);
    file_.read (reinterpret_cast<char *> (&m_d->compressionFlag), 1);
    file_.read (reinterpret_cast<char *> (&m_d->info5), 1);
    file_.read (reinterpret_cast<char *> (&m_d->offset), 2);

    if (m_d->offset > 0)
    {
        m_d->name.resize (m_d->offset);
        file_.read (m_d->name.data (), m_d->offset);

        sanitizeName ();
    }

    IF_PLOG (plog::Severity::verbose)
    {
        file_.seekg (initPos);

        vector<byte> raw;
        raw.resize (12 + m_d->offset);

        file_.read (reinterpret_cast<char *> (raw.data ()), 12 + m_d->offset);

        string const output =
            "\t\tFile Position: " + to_string (initPos) +
            "\n\t\tDisk Size: " + to_string (m_d->diskSize) +
            "\n\t\tUncompressed Size: " + to_string (m_d->uncompressedSize) +
            "\n\t\tOffset: " + to_string (m_d->offset) +
            "\n\t\tCompression Flag: " + to_string (m_d->compressionFlag) +
            "\n\t\tInfo 5: " + to_string (m_d->info5) + "\n\t\tRaw: ";

        PLOGV << "\n\tPAK INFO\n"
              << output << plog::hexdump (raw) << "\n\t\tName: " << m_d->name;
    }

    unpak (file_);
}

vector<byte> const &pakInfo_t::data () const
{
    return m_d->data;
}

void pakInfo_t::unpak (fstream &file_)
{
    switch (m_d->compressionFlag)
    {
    case 0:
    {
        m_d->data.resize (m_d->diskSize);
        file_.read (reinterpret_cast<char *> (m_d->data.data ()), m_d->diskSize);
        return;
    }
    case 1:
    {
        vector<byte> compressedDataPtr;

        compressedDataPtr.resize (m_d->diskSize);

        file_.read (reinterpret_cast<char *> (compressedDataPtr.data ()), m_d->diskSize);

        m_d->data.resize (m_d->uncompressedSize);

        PAK_explode (reinterpret_cast<unsigned char *> (compressedDataPtr.data ()),
                     reinterpret_cast<unsigned char *> (m_d->data.data ()),
                     m_d->diskSize,
                     m_d->uncompressedSize,
                     m_d->info5);
        return;
    }
    case 4:
    {
        vector<byte> compressedDataPtr;

        compressedDataPtr.resize (m_d->diskSize);

        file_.read (reinterpret_cast<char *> (compressedDataPtr.data ()), m_d->diskSize);

        m_d->data.resize (m_d->uncompressedSize);

        PAK_deflate (reinterpret_cast<unsigned char *> (compressedDataPtr.data ()),
                     reinterpret_cast<unsigned char *> (m_d->data.data ()),
                     m_d->diskSize,
                     m_d->uncompressedSize);
        return;
    }
    };
}

void pakInfo_t::sanitizeName ()
{
    list<string::iterator> toErase;
    bool foundChar = false;

    for (auto iter = m_d->name.end () - 1; iter != m_d->name.begin (); --iter)
    {
        if (*iter > 32)
            continue;
        if (foundChar)
        {
            *iter = 95;
        }
        else
        {
            m_d->name.erase (iter);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
class pak_t::private_t
{
public:
    ~private_t () = default;
    private_t ()
        : paks ()
    {
    }

    vector<pakInfo_t> paks;
};
pak_t::~pak_t () = default;
pak_t::pak_t (filesystem::path const &file_)
    : m_d (make_shared<private_t> ())
{
    PLOGD << filesystem::absolute (file_).string ();

    fstream iFile (filesystem::absolute (file_).string (), ios::binary | ios::in);

    iFile.seekg (4, ios::cur);

    uint32_t fileOffset;

    iFile.read (reinterpret_cast<char *> (&fileOffset), 4);
    uint32_t iFileCount = (fileOffset / 4) - 2;

    PLOGD << "File Count: " << to_string (iFileCount);

    m_d->paks.resize (iFileCount);

    uint8_t index = 1;
    int32_t additionalDescriptorSize;
    for (auto &pak : m_d->paks)
    {
        PLOGV << "Reading Pak " << to_string (index);
        // Goto PAK at index
        iFile.seekg (index * 4);

        // Read its offset
        iFile.read (reinterpret_cast<char *> (&fileOffset), 4);

        // Goto offset
        iFile.seekg (fileOffset);

        // Read any additional descriptor size
        iFile.read (reinterpret_cast<char *> (&additionalDescriptorSize), 4);

        // Read PAK info
        pak.init (iFile);

        ++index;
    }
}

vector<byte> const &pak_t::data (uint8_t index_) const
{
    return m_d->paks.at (index_).data ();
}
