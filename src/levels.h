#pragma once

#include <cstdint>

#include "arena.h"
#include "entity.h"

using namespace Memory;

struct LevelData{
  int w;
  int h;
  uint8_t* cells;
  const char* levelPath;
  Entity* entityBuffer;
  int entityCount;

};

void CreateLevel(Arena* arena, LevelData* level, const char* levelName);
void CreateEntities(LevelData* lvlData, Arena* arena);

Entity* GetNextAvailableEntitySlot(LevelData* lvl);
void AddEntity(ID entityID, int x, int y, LevelData* lvl);
void RemoveEntity(int x, int y, LevelData* lvl);


uint8_t getCellID(LevelData* lvl, int x, int y);
Entity* GetEntity(LevelData* lvl, int x, int y);
Entity* RaycastFirstEntity(int originX, int originY, Direction direction, LevelData* lvl, bool ignoreWalls = false);

