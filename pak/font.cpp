#include "font.h"
#include "buffer.h"

#include <plog/Log.h>

using namespace std;

class font_t::private_t
{
public:
    ~private_t () = default;
    private_t () {}
};

font_t::~font_t () = default;
font_t::font_t ()
    : m_d (make_shared<private_t> ())
{
}

void font_t::init (pak_t const &pak_)
{
    int16_t tempDx;
    int16_t tempAxFlip;

    buffer_t buffer (pak_.data ());

    auto fontVar1 = pak_.data (); // fontPtr

    tempDx = buffer.get<int16_t> (); // alignement

    uint8_t fontSm1 = buffer.get<uint8_t> (); // character height
    uint8_t fontSm2 = buffer.get<uint8_t> (); // character size

    if (!fontSm2)
    {
        fontSm2 = buffer.get<int16_t> ();
    }

    tempAxFlip = buffer.get<int16_t> ();

    tempAxFlip = ((tempAxFlip & 0xFF) << 8) | ((tempAxFlip & 0xFF00) >> 8);

    PLOGD << "Size: " << pak_.data ().size ();
    PLOGD << "TempDX: " << tempDx;
    PLOGD << "TempAX: " << tempAxFlip;
    PLOGD << "FontSm1: " << to_string (fontSm1);
    PLOGD << "FontSm2: " << to_string (fontSm2);
    PLOGD << tempAxFlip - (tempDx & 0xFF) * 2;
    auto fontVar4 = pak_.data ();

    auto fontVar5 = vector<byte> (
        pak_.data ().begin () + tempAxFlip - (tempDx & 0xFF) * 2, pak_.data ().end ());

    PLOGD << fontVar5.size ();
    // currentFontColor = color;

    // fontSm3 = color;

    int fontVar6 = 0;
    int fontSm7 = 0;
    int16_t fontSm9 = 0x80;

    string test = "abcz";

    for (auto character : test)
    {
        uint16_t data;
        uint16_t dx;

        buffer_t dataPtr (
            vector<byte> (fontVar5.begin () + character * 2, fontVar5.end ()));

        data = dataPtr.get<uint16_t> ();
        PLOGD << "DATA: " << data;

        data = ((data & 0xFF) << 8) | ((data & 0xFF00) >> 8);
        PLOGD << "FLIP: " << data;

        dx = data;

        data >>= 12;
        PLOGD << "shift: " << data;

        if (data & 0xF) // real character (width != 0)
        {
            uint8_t *characterPtr;
            int bp;
            int ch;

            dx &= 0xFFF;

            characterPtr = (dx >> 3) + reinterpret_cast<uint8_t *> (fontVar4.front ());

            /*
            fontSm9 = flagTable[dx & 7];

            bp = fontSm7;

            fontSm8 = fontVar6;

            ch;

            for (ch = fontSm1; ch > 0; ch--)
            {
                if (bp >= 200)
                    return;
                char *outPtr = logicalScreen + bp * 320 + fontSm8;

                int dh = fontSm9;
                int cl = data & 0xF;

                int al = *characterPtr;

                int bx;

                bp++;

                for (bx = 0; cl > 0; cl--)
                {
                    if (dh & al)
                    {
                        *(outPtr) = (char)fontSm3;
                    }

                    outPtr++;

                    dh = ((dh >> 1) & 0x7F) | ((dh << 7) & 0x80);

                    if (dh & 0x80)
                    {
                        bx++;
                        al = *(characterPtr + bx);
                    }
                }

                characterPtr += fontSm2;
            }

            fontVar6 += data & 0xF;
            */
        }
        else // space character
        {
            //            fontVar6 += g_fontInterWordSpace;
        }

        //       fontVar6 += g_fontInterLetterSpace;
    }
}
