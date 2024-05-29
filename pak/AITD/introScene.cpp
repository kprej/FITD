#include "AITD/introScene.h"
#include "AITD/AITD.h"
#include "AITD/resources.h"

#include "body.h"
#include "osystem.h"
#include "texture.h"

#include <imgui.h>
#include <plog/Log.h>

#include <map>
#include <string>
using namespace std;

namespace
{
enum state_t
{
    ENTER,
    INFOGRAM,
    DILLO,
    EXIT,
    FINISHED,
};
}

class introScene_t::private_t
{
public:
    ~private_t () {}
    private_t ()
        : tatou ()
        , backgroundTexture (texture_t::FULLSCREEN)
        , foregroundTexture (texture_t::FULLSCREEN)
        , state (ENTER)
        , lastState (ENTER)
        , sceneTime (0)
        , shrinkStep (5500)
        , shrinkTimeMSec (5500)
    {
    }

    body_t tatou;
    texture_t backgroundTexture;
    texture_t foregroundTexture;

    state_t state;
    state_t lastState;

    uint16_t sceneTime;

    float shrinkStep;
    float shrinkTimeMSec;
};

introScene_t::~introScene_t ()
{
}

introScene_t::introScene_t ()
    : m_d (make_shared<private_t> ())
{
    GS ()->palettes[resources_t::PAL_TATOU] = texture_t (
        texture_t::PALETTE, GS ()->paks.at ("ITD_RESS").data (aitd_t::ress_t::TATOU_PAL));

    m_d->tatou.parseData (GS ()->paks.at ("ITD_RESS").data (aitd_t::ress_t::TATOU_3DO));
    m_d->tatou.setPalette (resources_t::PAL_TATOU);

    m_d->tatou.rotateY (-90);
    m_d->tatou.setPos (0.f, 0.f, 0.f);

    auto tCenter = m_d->tatou.center ();

    GS ()->camera.lookAt (tCenter);

    GS ()->camera.setFOV (8);

    GS ()->camera.setPos ({0.0f, tCenter.y, 190.f});
    GS ()->camera.setViewSize ({864, 540});
    GS ()->camera.setViewPos ({208, 0});

    m_d->backgroundTexture.update (
        GS ()->paks.at ("ITD_RESS").data (aitd_t::ress_t::TATOU_MCG), 770);

    m_d->foregroundTexture.fill (3);

    m_d->foregroundTexture.setPalette (resources_t::PAL_TATOU);
    m_d->backgroundTexture.setPalette (resources_t::PAL_TATOU);

    GS ()->debug.draw.connect<&introScene_t::debug> (this);
}

bool introScene_t::run ()
{
    m_d->sceneTime += GS ()->delta;

    switch (m_d->state)
    {
    case ENTER:
        enter ();
        break;
    case INFOGRAM:
        infogram ();
        break;
    case DILLO:
        dillo ();
        break;
    case EXIT:
        // exit ();
        break;
    case FINISHED:
        // return false;
    }

    return true;
}

void introScene_t::enter ()
{
    GS ()->handle.fadeIn (1000);

    m_d->state = INFOGRAM;
}

void introScene_t::infogram ()
{
    GS ()->handle.drawFullscreenBackground (m_d->backgroundTexture);
    // if (GS ()->handle.fadeState () != fadeState_t::VISIBLE)
    return;

    /*
    if (IN ()->anyKey)
    {
        GS ()->handle.fadeOut (1000);
        m_d->state = EXIT;
        m_d->lastState = INFOGRAM;
        return;
    }
    */

    m_d->backgroundTexture.fill (0, {0, 0, 320, 140});

    m_d->state = DILLO;
    GS ()->samples.at (6).play ();
    m_d->sceneTime = 0;
}

void introScene_t::dillo ()
{
    GS ()->handle.drawFullscreenBackground (m_d->backgroundTexture);
    if (m_d->sceneTime < 50)
    {
        GS ()->handle.drawFullscreenForeground (m_d->foregroundTexture);
        return;
    }

    GS ()->handle.drawBody (m_d->tatou);

    if (IN ()->anyKey)
    {
        // GS ()->handle.fadeOut (1000);
        // m_d->state = EXIT;
        ////m_d->lastState = DILLO;
        ////return;
    }

    m_d->tatou.rotateY (-0.2);
    m_d->shrinkStep -= GS ()->delta;
    m_d->tatou.setScale (glm::clamp (
        lerp (0.0f, 1.0f, m_d->shrinkStep / m_d->shrinkTimeMSec), 0.0f, 1.0f));

    if (!GS ()->samples.at (6).isPlaying ())
    {
        // GS ()->handle.fadeOut (1000);
        //  m_d->state = EXIT;
        //  m_d->lastState = DILLO;
    }
}

void introScene_t::exit ()
{
    if (GS ()->handle.fadeState () == fadeState_t::INVISIBLE)
    {
        GS ()->debug.draw.disconnect<&introScene_t::debug> (this);
        m_d->state = FINISHED;
        m_d->lastState = EXIT;
        return;
    }

    if (m_d->lastState == DILLO)
        GS ()->handle.drawBody (m_d->tatou);

    GS ()->handle.drawFullscreenBackground (m_d->backgroundTexture);
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
