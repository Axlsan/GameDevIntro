#include <cstddef>
#include <cstdint>
#include <fstream>
#include <new>
#include <vector>

#include "Parsers/json.hpp"
#include "levels.h"
#include "arena.h"
#include "entity.h"

using namespace std;

const int LEVEL_INDEX = 0;
const int ENTITIES_INDEX = 1;

void CreateLevel(Arena* arena, LevelData* level, const char* levelName){
  fstream stream(levelName);
  auto jsonResult = nlohmann::json::parse(stream);
  vector dataField = jsonResult["layers"][LEVEL_INDEX]["data"].get<vector<uint8_t>>();

  level->w = jsonResult["width"].get<int>();
  level->h = jsonResult["height"].get<int>();
  level->levelPath = levelName;
  size_t size_of_cells = sizeof(uint8_t) * level->w * level->h;

  level->cells = (uint8_t*)Memory::Allocate(arena, size_of_cells);

  for (int i = 0; i < level->w * level->h; i++) {
    level->cells[i] = dataField[i];
  }

}

void CreateEntities(LevelData* lvlData, Arena* arena){
  Reset(arena);
  lvlData->entityCount = 0;
  fstream stream(lvlData->levelPath);
  auto result = nlohmann::json::parse(stream);
  auto entityData = result["layers"][ENTITIES_INDEX]["data"].get<vector<uint8_t>>();

  
  lvlData->entityBuffer = (Entity*)Memory::Allocate(arena, sizeof(Entity) * lvlData->entityCount);


  for(int i = 0; i< lvlData->w; i++){
    unsigned char entityID = entityData[i];
    if(entityID != 0){
      int x = i % lvlData->w;
      int y = i / lvlData->w;
      AddEntity((ID)entityID, x, y, lvlData);
    }
  }
  /*
  // En schwanky lösning
  int index = 0;
  for(int i = 0; i < lvlData->w * lvlData->h; i++){
    unsigned char entityID = entityData[i];
    if(entityID != 0){
      lvlData->entityCount++;

      
      lvlData->entityBuffer[index].id = (ID)entityID;
      lvlData->entityBuffer[index].InitializeBaseBehaviour();
      lvlData->entityBuffer[index].x = i % lvlData->w;
      lvlData->entityBuffer[index].y = i / lvlData->w;
      index++;

    }
  }
  */
}


uint8_t getCellID(LevelData* lvl, int x, int y){
  return lvl->cells[y * lvl->w + x];
}

Entity* GetEntity(LevelData* lvl, int x, int y){
  for(int i = 0; i < lvl->entityCount; i++){
    if(lvl->entityBuffer[i].x == x && lvl->entityBuffer[i].y == y){
      return &lvl->entityBuffer[i];
    }
  }
  return nullptr;
}


Entity* GetNextAvailableEntitySlot(LevelData* lvl){
  for(int i = 0; i < lvl->entityCount; i++){
    if(lvl->entityBuffer[i].id == ID::NONE){
      return &lvl->entityBuffer[i];
    }
  }
  return &lvl->entityBuffer[lvl->entityCount++];
}

void AddEntity(ID entityID, int x, int y, LevelData* lvl){
  Entity* entity = GetEntity(lvl, x, y);

  if(entity == nullptr){
    entity = GetNextAvailableEntitySlot(lvl);
  }

  entity->x = x;
  entity->y = y;
  entity->xPrev = x;
  entity->yPrev = y;
  entity->id = entityID;
  InitializeBaseBehaviour(entity);
}

void RemoveEntity(int x, int y, LevelData* lvl){
  Entity* entity = GetEntity(lvl, x, y);
  if(entity == nullptr){
    return;
  }

  *entity = {}; //nullptr?
}

Entity* RaycastFirstEntity(int originX, int originY, Direction direction, LevelData* lvl, bool ignoreWalls){
  Position facingVector;
  switch(direction){
    case Direction::RIGHT:
      facingVector = {1,0};
      break;
    case Direction::LEFT:
      facingVector = {-1,0};
      break;
    case Direction::UP:
      facingVector = {0,1};
      break;
    case Direction::DOWN:
      facingVector = {0,-1};
      break;
  }

  int searchX = originX + facingVector.x;
  int searchY = originY + facingVector.y;

  while(searchX > 0 && searchX < lvl->w && searchY < lvl->h){
    ID cellID = (ID)getCellID(lvl, searchX, searchY);
    if(cellID == ID::WALL && !ignoreWalls){
      break;
    }

    Entity* entitySearch = GetEntity(lvl, searchX, searchY);
    if(entitySearch != nullptr){
      return entitySearch;
    }

    searchX += facingVector.x;
    searchY += facingVector.y;
  }

  return nullptr;
} 

