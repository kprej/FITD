#include "AITD/AITD.h"
#include "AITD/LISTMUS.h"

#include "body.h"
#include "introScene.h"
#include "osystem.h"
#include "pakFile.h"
#include "sound.h"

#include <plog/Helpers/HexDump.h>
#include <plog/Log.h>

#include <map>
using namespace std;

class aitd_t::private_t
{
public:
    ~private_t () = default;
    private_t ()
        : state (state_t::INTRO_TATOU)
    {
    }

    state_t state;
    introScene_t intro;
};

aitd_t::~aitd_t ()
{
}

aitd_t::aitd_t ()
    : m_d (make_shared<private_t> ())
{
}

void aitd_t::_init ()
{
    for (auto const &pak : GS ()->paks.at ("LISTSAMP").paks ())
    {
        sound_t sample;
        sample.init (pak.data ());

        GS ()->samples.push_back (sample);
    }

    MP ()->setMusicPak (GS ()->paks.at ("LISTMUS"));
    MP ()->setTrackNames (aitd::TRACK_NAMES);
}

void aitd_t::_start ()
{
    switch (m_d->state)
    {
    case state_t::INTRO_TATOU:
        m_d->intro.run ();
        break;
    case state_t::INTRO_SCREEN:
        break;
    }
    //    makeIntroScreens ();
}

void aitd_t::_readBook (int index_, int type_)
{
}

void aitd_t::makeIntroScreens ()
{
    GS ()->handle.setPalette (GS ()->paks.at ("ITD_RESS").data (ress_t::PALETTE_GAME));
    GS ()->handle.setBackground (GS ()->paks.at ("ITD_RESS").data (ress_t::TITLE), 770);
}

void aitd_t::drawBox (int x_, int y_, int width_, int height_)
{
}
