#include "AITD/titleScene.h"
#include "AITD/AITD.h"

#include "body.h"
#include "osystem.h"

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
    TITLE,
    DILLO,
    EXIT,
    FINISHED,
};
}

class titleScene_t::private_t
{
public:
    ~private_t () = default;
    private_t ()
        : state (ENTER)
        , lastState (ENTER)
        , sceneTime (0)
        , shrinkStep (5500)
        , shrinkTimeMSec (5500)
    {
    }

    state_t state;
    state_t lastState;
    uint16_t sceneTime;
    float shrinkStep;
    float shrinkTimeMSec;
};

titleScene_t::~titleScene_t ()
{
}

titleScene_t::titleScene_t ()
    : m_d (make_shared<private_t> ())
{
    GS ()->debug.draw.connect<&titleScene_t::debug> (this);
}

bool titleScene_t::run ()
{
    m_d->sceneTime += GS ()->delta;

    switch (m_d->state)
    {
    case ENTER:
        enter ();
        break;
    case TITLE:
        title ();
        break;
    case DILLO:
        dillo ();
        break;
    case EXIT:
        exit ();
        break;
    case FINISHED:
        return false;
    }

    return true;
}

void titleScene_t::enter ()
{
    GS ()->handle.fadeIn (500);
    GS ()->handle.setPalette (
        GS ()->paks.at ("ITD_RESS").data (aitd_t::ress_t::PALETTE_GAME));
    GS ()->backgroundView.update (
        GS ()->paks.at ("ITD_RESS").data (aitd_t::ress_t::FRAME_SPF), 0);

    m_d->state = TITLE;
}

void titleScene_t::title ()
{
}

void titleScene_t::dillo ()
{
}

void titleScene_t::exit ()
{
}

void titleScene_t::debug ()
{
    if (ImGui::Begin ("Title Scene"))
    {
    }
    ImGui::End ();
}
