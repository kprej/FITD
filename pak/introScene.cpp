#include "introScene.h"
#include "AITD.h"
#include "body.h"
#include "osystem.h"

#include <map>
#include <string>
using namespace std;

namespace
{
enum state_t
{
    TITLE,
    DILLO,
    TRANSITION,
};
}

class introScene_t::private_t
{
public:
    ~private_t () = default;
    private_t ()
        : bodies ()
        , state (TITLE)
        , fade (0)
    {
    }

    map<string, body_t> bodies;
    state_t state;

    float fade;
};

introScene_t::~introScene_t ()
{
}

introScene_t::introScene_t ()
    : m_d (make_shared<private_t> ())
{
    m_d->bodies["tatou"].parseData (
        GS ()->paks.at ("ITD_RESS").data (aitd_t::ress_t::TATOU_3DO));

    m_d->bodies["tatou"].rotateZ (180);
    GS ()->handle.fadeInBackground (m_d->fade);
}

void introScene_t::run ()
{
    switch (m_d->state)
    {
    case TITLE:
        if (GS ()->handle.backgroundState () == backgroundState_t::FADING_IN)
        {
            GS ()->handle.fadeInBackground (m_d->fade++);
        }

        GS ()->handle.setBackground (
            GS ()->paks.at ("ITD_RESS").data (aitd_t::ress_t::TATOU_MCG), 770);

        GS ()->handle.setPalette (
            GS ()->paks.at ("ITD_RESS").data (aitd_t::ress_t::TATOU_PAL));

        if (IN ()->anyKey)
        {
            m_d->state = TRANSITION;
            m_d->fade = 255;
            GS ()->handle.fadeOutBackground (m_d->fade);
        }
        break;
    case DILLO:
        GS ()->handle.drawBody (m_d->bodies["tatou"]);
        break;
    case TRANSITION:
        if (GS ()->handle.backgroundState () == backgroundState_t::FADING_OUT)
        {
            GS ()->handle.fadeOutBackground (m_d->fade--);
        }

        GS ()->handle.setBackground (
            GS ()->paks.at ("ITD_RESS").data (aitd_t::ress_t::TATOU_MCG), 770);

        GS ()->handle.setPalette (
            GS ()->paks.at ("ITD_RESS").data (aitd_t::ress_t::TATOU_PAL));
        break;
    }
}
