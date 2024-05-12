#include "introScene.h"
#include "AITD.h"
#include "body.h"
#include "osystem.h"

#include <imgui.h>

#include <map>
#include <string>
using namespace std;

namespace
{
enum state_t
{
    ENTER,
    TITLE,
    DILLO,
    EXIT,
};
}

class introScene_t::private_t
{
public:
    ~private_t () = default;
    private_t ()
        : tatou ()
        , state (ENTER)
        , introTime (0)
    {
    }

    body_t tatou;
    state_t state;
    uint16_t introTime;
};

introScene_t::~introScene_t ()
{
}

introScene_t::introScene_t ()
    : m_d (make_shared<private_t> ())
{
    m_d->tatou.parseData (GS ()->paks.at ("ITD_RESS").data (aitd_t::ress_t::TATOU_3DO));

    m_d->tatou.rotateY (90);
    m_d->tatou.scale (0.45f);

    GS ()->debug.draw.connect<&introScene_t::debug> (this);
}

void introScene_t::enter ()
{
    GS ()->handle.fadeInBackground (1000);
    GS ()->handle.setBackground (
        GS ()->paks.at ("ITD_RESS").data (aitd_t::ress_t::TATOU_MCG), 770);

    GS ()->handle.setPalette (
        GS ()->paks.at ("ITD_RESS").data (aitd_t::ress_t::TATOU_PAL));

    m_d->state = TITLE;
}

void introScene_t::run ()
{
    m_d->introTime += GS ()->delta;
    switch (m_d->state)
    {
    case ENTER:
        enter ();
        break;
    case TITLE:
        if (GS ()->handle.backgroundState () == backgroundState_t::FADING_IN)
            return;

        if (IN ()->anyKey || m_d->introTime > 1000)
        {
            m_d->state = DILLO;
            GS ()->samples.at (6).play ();
            return;
        }

        break;
    case DILLO:
        GS ()->handle.drawBody (m_d->tatou);
        m_d->tatou.rotateY (0.2);
        break;
    case EXIT:
        exit ();
        break;
    }
}

void introScene_t::exit ()
{
    GS ()->handle.fadeOutBackground (1000);
}

void introScene_t::debug ()
{
    if (ImGui::Begin ("Intro Scene"))
    {
        if (ImGui::TreeNode ("Tatou"))
        {
            m_d->tatou.debug ();
            ImGui::TreePop ();
        }
    }
    ImGui::End ();
}
