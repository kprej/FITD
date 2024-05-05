#pragma once

#include "game.h"

class aitd_t : public game_t
{
public:
    enum ress_t
    {
        TATOU_3DO = 0,
        TATOU_PAL = 1,
        TATOU_MCG = 2,
        PALETTE_JEU = 3,
        CADRE_SPF = 4,
        ITDFONT = 5,
        LETTRE = 6,
        LIVRE = 7,
        CARNET = 8,
        TEXT_GRAPH = 9,
        PERSO_CHOICE = 10,
        GRENOUILLE = 11,
        DEAD_END = 12,
        TITRE = 13,
        FOND_INTRO = 14,
        CAM07000 = 15,
        CAM07001 = 16,
        CAM06000 = 17,
        CAM06005 = 18,
        CAM06008 = 19,
    };

    ~aitd_t ();
    aitd_t ();

private:
    void _start () final;
    void _readBook (int index_, int type_) final;
};
