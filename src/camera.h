#pragma once

#include "levels.h"

struct Camera{
  float cameraX;
  float cameraY;
};

namespace camera{
  void GridToWorld(float* x, float* y, const LevelData* lvl);
  void WorldToGrid(float worldX, float worldY, int* x, int* y, const LevelData* lvl);
  bool IsPointInGrid(float x, float y, const LevelData* lvl);
  
}
