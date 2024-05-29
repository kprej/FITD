#include "AITD/AITD.h"
#include "AITD/LISTMUS.h"
#include "AITD/introScene.h"
#include "AITD/resources.h"
#include "AITD/titleScene.h"

#include "frame.h"
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
        , intro (nullptr)
        , title (nullptr)
        , deferStartTime (0)
    {
    }

    state_t state;
    unique_ptr<introScene_t> intro;
    unique_ptr<titleScene_t> title;

    frame_t frame;

    uint16_t deferStartTime;
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

    GS ()->palettes.insert (
        {resources_t::PAL_GAME,
         texture_t (texture_t::PALETTE,
                    GS ()->paks.at ("ITD_RESS").pak (ress_t::PALETTE_GAME).data ())});

    MP ()->setMusicPak (GS ()->paks.at ("LISTMUS"));
    MP ()->setTrackNames (aitd::TRACK_NAMES);

    GS ()->handle.createFontTexture (16, 1797);
    GS ()->font.init (GS ()->paks.at ("ITD_RESS").pak (ress_t::ITDFONT), 1797);

    m_d->frame.setTexture (GS ()->paks.at ("ITD_RESS").pak (ress_t::FRAME_SPF).data ());

    m_d->intro = make_unique<introScene_t> ();
}

void aitd_t::_start ()
{
    if (m_d->deferStartTime < 1000)
    {
        m_d->deferStartTime += GS ()->delta;
        return;
    }

    m_d->frame.render ({0, 0, 0, 0});
    switch (m_d->state)
    {
    case state_t::INTRO_TATOU:
        if (!m_d->intro->run ())
        {
            m_d->state = state_t::TITLE_SCREEN;
            m_d->intro.reset (nullptr);
            m_d->title = make_unique<titleScene_t> ();
        }
        break;
    case state_t::TITLE_SCREEN:
        if (!m_d->title->run ())
        {
        }
        break;
    }
}

void aitd_t::_readBook (int index_, int type_)
{
}

void aitd_t::makeIntroScreens ()
{
}

void aitd_t::drawBox (int x_, int y_, int width_, int height_)
{
}
