#include "font.h"
#include "buffer.h"
#include "osystem.h"

#include <plog/Log.h>

#include "bgfxHandle.h"

using namespace std;

namespace
{
unsigned char flagTable[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
struct character_t
{
    uint8_t width;
    uint8_t height;
    float bottomLeftCorner[2];
    float topRightCorner[2];
};
} // namespace

class font_t::private_t
{
public:
    ~private_t () = default;
    private_t () {}

    map<uint8_t, character_t> characters;
};

font_t::~font_t () = default;
font_t::font_t ()
    : m_d (make_shared<private_t> ())
{
}

void font_t::init (pak_t const &pak_, uint16_t textureWidth_)
{
    int16_t tempDx;
    int16_t tempAxFlip;

    buffer_t buffer (pak_.data ());

    tempDx = buffer.get<int16_t> (); // alignement

    uint8_t characterHeight = buffer.get<uint8_t> ();
    uint8_t characterWidth = buffer.get<uint8_t> ();

    if (!characterWidth)
    {
        characterWidth = buffer.get<int16_t> ();
    }

    tempAxFlip = buffer.get<int16_t> ();

    tempAxFlip = ((tempAxFlip & 0xFF) << 8) | ((tempAxFlip & 0xFF00) >> 8);

    auto characterLocation = pak_.data ().begin () + 8;

    auto fontVar5 = pak_.data ().begin () + (tempAxFlip - (tempDx & 0xFF) * 2);

    int16_t fontSm9 = 0x80;

    uint32_t xOffset = 0;

    for (unsigned char i = 0; i < 255; ++i)
    {
        uint16_t data;
        uint16_t characterOffset;

        auto c = fontVar5 + i * 2;

        data = (uint8_t (*c) | uint8_t (*(c + 1)) << 8);
        data = ((data & 0xFF) << 8) | ((data & 0xFF00) >> 8);

        characterOffset = data;

        data >>= 12;

        if (data & 0xF) // real character (width != 0)
        {
            int bp = 0;

            int cl = data & 0xF;

            uint8_t width = cl;

            character_t character;

            character.width = cl;
            character.height = characterHeight;

            characterOffset &= 0xFFF;
            auto characterPtr = (characterLocation + (characterOffset >> 3));

            fontSm9 = flagTable[characterOffset & 7];

            vector<byte> outChar;
            for (int ch = characterHeight; ch > 0; ch--)
            {
                if (bp >= 200)
                    return;

                int dh = fontSm9;

                int al = int (*characterPtr);

                int bx = 0;

                bp++;

                for (int cl = width; cl > 0; cl--)
                {
                    if (dh & al)
                        outChar.push_back (byte (1));
                    else
                        outChar.push_back (byte (0));

                    dh = ((dh >> 1) & 0x7F) | ((dh << 7) & 0x80);

                    if (dh & 0x80)
                    {
                        bx++;
                        al = int (*(characterPtr + bx));
                    }
                }
                characterPtr += characterWidth;
            }

            if (!outChar.empty ())
            {
                GS ()->handle.addText (outChar, xOffset, width);

                character.bottomLeftCorner[0] = float (xOffset) / float (textureWidth_);
                character.bottomLeftCorner[1] = 0.f;

                character.topRightCorner[0] =
                    float (xOffset + width) / float (textureWidth_);
                character.topRightCorner[1] = 1.f;

                m_d->characters[i] = character;

                xOffset += width;
            }
        }
    }
}

void font_t::render (string const &text_)
{
}
