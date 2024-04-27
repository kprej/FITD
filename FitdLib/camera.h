#pragma once

#include <cstdint>

struct rectTest_t
{
    int16_t zoneX1;
    int16_t zoneZ1;
    int16_t zoneX2;
    int16_t zoneZ2;
};

struct cameraZonePoint_t
{
    int16_t x;
    int16_t y;
};

struct cameraZoneEntry_t
{
    uint16_t numPoints;

    cameraZonePoint_t *pointTable;
};

struct cameraMask_t
{
    uint16_t numTestRect;
    rectTest_t *rectTests;
};

struct cameraViewedRoom_t
{
    int16_t viewedRoomIdx;
    int16_t offsetToMask;
    int16_t offsetToCover;
    int16_t offsetToHybrids;
    int16_t offsetCamOptims;
    int16_t lightX;
    int16_t lightY;
    int16_t lightZ;

    uint16_t numMask;
    cameraMask_t *masks;
    uint16_t numCoverZones;
    cameraZoneEntry_t *coverZones;
};

struct cameraData_t
{
    int16_t alpha;
    int16_t beta;
    int16_t gamma;

    int16_t x;
    int16_t y;
    int16_t z;

    int16_t focal1;
    int16_t focal2;
    int16_t focal3;

    uint16_t numViewedRooms;
    cameraViewedRoom_t *viewedRoomTable;
};
