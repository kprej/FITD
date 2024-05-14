#include "font.h"
#include "buffer.h"

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

    // fontVar1 = fontData; // fontPtr

    tempDx = buffer.get<int16_t> (); // alignement

    uint8_t fontSm1 = buffer.get<uint8_t> (); // character height
    uint8_t fontSm2 = buffer.get<uint8_t> (); // character size

    if (!fontSm2)
    {
        fontSm2 = buffer.get<int16_t> ();
    }

    tempAxFlip = buffer.get<int16_t> ();

    tempAxFlip = ((tempAxFlip & 0xFF) << 8) | ((tempAxFlip & 0xFF00) >> 8);

    // fontVar4 = fontData;

    // fontVar5 = fontVar1 + tempAxFlip - (tempDx & 0xFF) * 2;

    // currentFontColor = color;

    // fontSm3 = color;
}
