#pragma once

enum class gameId_t
{
    AITD1,
    JACK,
    AITD2,
    AITD3,
    TIMEGATE,
};

enum class backgroundMode_t
{
    _2D,
    _3D,
};

enum CVars_t
{
    SAMPLE_PAGE,
    BODY_FLAMME,
    MAX_WEIGHT_LOADABLE,
    TEXTE_CREDITS,
    SAMPLE_TONNERRE,
    INTRO_DETECTIVE,
    INTRO_HERITIERE,
    WORLD_NUM_PERSO,
    CHOOSE_PERSO,
    SAMPLE_CHOC,
    SAMPLE_PLOUF,
    REVERSE_OBJECT,
    KILLED_SORCERER,
    LIGHT_OBJECT,
    FOG_FLAG,
    DEAD_PERSO,
    JET_SARBACANE,
    TIR_CANON,
    JET_SCALPEL,
    POIVRE,
    DORTOIR,
    EXT_JACK,
    NUM_MATRICE_PROTECT_1,
    NUM_MATRICE_PROTECT_2,
    NUM_PERSO,
    TYPE_INVENTAIRE,
    PROLOGUE,
    POIGNARD,
    MATRICE_FORME,
    MATRICE_COULEUR,

    UNKNOWN_CVAR // for table padding, shouldn't be called !
};
