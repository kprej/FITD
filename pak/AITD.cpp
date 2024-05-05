#include "AITD.h"
#include "osystem.h"
#include "pakFile.h"

#include <plog/Log.h>

aitd_t::~aitd_t ()
{
}

aitd_t::aitd_t ()
{
}

void aitd_t::_start ()
{
    GS ()->handle.setPalette (GS ()->paks.at ("ITD_RESS").data (ress_t::PALETTE_GAME));
    makeIntroScreens ();
}

void aitd_t::_readBook (int index_, int type_)
{
}

void aitd_t::makeIntroScreens ()
{
    GS ()->handle.setBackground (GS ()->paks.at ("ITD_RESS").data (ress_t::TITLE), 770);
}
