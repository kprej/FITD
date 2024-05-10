#pragma once

#include "game.h"

#include <memory>

class aitd_t : public game_t
{
public:
    enum ress_t
    {
        TATOU_3DO = 0,
        TATOU_PAL = 1,
        TATOU_MCG = 2,
        PALETTE_GAME = 3,
        FRAME_SPF = 4,
        ITDFONT = 5,
        LETTER = 6,
        BOOK = 7,
        NOTEBOOK = 8,
        TEXT_GRAPH = 9,
        PERSO_CHOICE = 10,
        FROG = 11,
        DEAD_END = 12,
        TITLE = 13,
        BOTTOM_INTRO = 14,
        CAM07000 = 15,
        CAM07001 = 16,
        CAM06000 = 17,
        CAM06005 = 18,
        CAM06008 = 19,
    };
    enum class state_t
    {
        INTRO_TATOU,
        INTRO_SCREEN,
    };

    ~aitd_t ();
    aitd_t ();

private:
    void _start () final;
    void _readBook (int index_, int type_) final;

    void drawBox (int x_, int y_, int width_, int height_);

    void makeIntroScreens ();

    class private_t;
    std::shared_ptr<private_t> m_d;
};
