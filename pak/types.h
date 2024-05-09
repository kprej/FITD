#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct point_t
{
    int16_t x;
    int16_t y;
    int16_t z;
};

struct sGroupState
{
    int16_t m_type;           // 8
    int16_t m_delta[3];       // A
    int16_t m_rotateDelta[3]; // 10 (AITD2+) if Info_optimise
};

struct sGroup
{
    int16_t m_start;        // 0
    int16_t m_numVertices;  // 2
    int16_t m_baseVertices; // 4
    int8_t m_orgGroup;      // 6
    int8_t m_numGroup;      // 7
    sGroupState m_state;
};

enum primitiveType_t
{
    Line = 0,
    Poly = 1,
    Point = 2,
    Sphere = 3,
    Disk = 4,
    Cylinder = 5,
    BigPoint = 6,
    Zixel = 7,
    PolyTexture8 = 8,
    PolyTexture9 = 9,
    PolyTexture10 = 10,
};

struct sPrimitive
{
    primitiveType_t m_type;
    uint8_t m_subType;
    uint8_t m_color;
    uint8_t m_even;
    uint16_t m_size;
    std::vector<uint16_t> m_points;
};

struct ZVStruct
{
    int32_t ZVX1;
    int32_t ZVX2;
    int32_t ZVY1;
    int32_t ZVY2;
    int32_t ZVZ1;
    int32_t ZVZ2;
};

enum class gameId_t
{
    AITD1,
    JACK,
    AITD2,
    AITD3,
    TIMEGATE,
};

extern std::string toString (gameId_t id_);

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
