#include "AITD.h"
#include "body.h"
#include "osystem.h"
#include "pakFile.h"

#include <plog/Log.h>

#include <map>
using namespace std;

class aitd_t::private_t
{
public:
    ~private_t () = default;
    private_t ()
        : bodies ()
    {
    }

    map<std::string, body_t> bodies;
};

aitd_t::~aitd_t ()
{
}

aitd_t::aitd_t ()
    : m_d (make_shared<private_t> ())
{
    m_d->bodies["tatou"].parseData (GS ()->paks.at ("ITD_RESS").data (ress_t::TATOU_3DO));
}

void aitd_t::_start ()
{
    GS ()->handle.setPalette (GS ()->paks.at ("ITD_RESS").data (ress_t::PALETTE_GAME));
    GS ()->handle.drawBody (m_d->bodies["tatou"]);
    makeIntroScreens ();
}

void aitd_t::_readBook (int index_, int type_)
{
}

void aitd_t::makeIntroScreens ()
{
    GS ()->handle.setBackground (GS ()->paks.at ("ITD_RESS").data (ress_t::TITLE), 770);
}

void aitd_t::drawBox (int x_, int y_, int width_, int height_)
{
}
