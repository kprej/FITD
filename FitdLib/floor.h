#pragma once

#include "camera.h"

#include <vector>

extern std::vector<cameraData_t> g_currentFloorCameraData;
extern uint32_t g_currentFloorRoomRawDataSize;
extern uint32_t g_currentFloorNumCamera;
void loadFloor (int floorNumber);
