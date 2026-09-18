#include "camera.h"
#include "common.h"

bool camera::IsPointInGrid(float x, float y, const LevelData *lvl){
  int gridX;
  int gridY;
  WorldToGrid(x, y, &gridX, &gridY, lvl);
  return gridX >= 0 && gridY >= 0 && gridX < lvl->w && gridY < lvl->h;
}

void camera::GridToWorld(float* x, float* y, const LevelData* lvl){
  *x *= CELL_SIZE_PX;
  *x += SCREEN_WIDTH / 2.0;
  *x -= lvl->w * CELL_SIZE_PX / 2;
  *y *= CELL_SIZE_PX;
  *y += SCREEN_HEIGHT / 2.0;
  *y -= lvl->h * CELL_SIZE_PX / 2;
}

void camera::WorldToGrid(float worldX, float worldY, int* x, int* y, const LevelData* lvl){
  *x = worldX;
  *y = worldY;
  *x += lvl->w * CELL_SIZE_PX / 2.0;
  *x -= SCREEN_WIDTH / 2.0;
  *x /= CELL_SIZE_PX;
  *y += lvl->h * CELL_SIZE_PX / 2.0;
  *y -= SCREEN_HEIGHT / 2.0;
  *y /= CELL_SIZE_PX;
}


