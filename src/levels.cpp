#include <cstdint>
#include <fstream>
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

/*
  for(int i = 0; i < lvlData->w * lvlData->h; i++){
    unsigned char entityID = entityData[i];
    if(entityID != 0){
      lvlData->entityCount++;
    }
  }
*/
  
  // En schwanky lösning
  lvlData->entityBuffer = (Entity*)Memory::Allocate(arena, sizeof(Entity) * lvlData->entityCount);
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
}
