#include <cstdint>
#include <iterator>
#include <cmath>

#include "levelRenderer.h"
#include "common.h"
#include "entity.h"
#include "levels.h"
#include "rendering.h"
#include "spriteLibrary.h"

void RenderLevel(GameData* gameData, SDL_Renderer* renderer){

  LevelData lvlData = gameData->levels[gameData->currentLevelIndex];

  //int boardWidthPxHalf = lvlData.w * CELL_SIZE_PX / 2;
  //int boardHeightPxHalf = lvlData.h * CELL_SIZE_PX / 2;

  for(int x = 0; x < lvlData.w; x++){
    for(int y = 0; y < lvlData.h; y++){
      uint8_t cellType = lvlData.getCellID(x, y);

      Sprite* sprite = GetSpriteFromID((ID)cellType, gameData->spriteBuffer);
      if(sprite == nullptr) continue;
      RenderSprite_Grid(sprite, &lvlData, renderer, &gameData->camera, x, y);
    }
  }
}

void RenderEntities(GameData* data, SDL_Renderer* renderer){
  LevelData lvlData = data->levels[data->currentLevelIndex];

    for(int i = 0; i < lvlData.entityCount; i++){
    Entity entity = lvlData.entityBuffer[i];
    Sprite* sprite = GetSpriteFromID(entity.id, data->spriteBuffer);
    
    // Animation
    float xAnimated = std::lerp(entity.xPrev, entity.x, entity.progress01);
    float yAnimated = std::lerp(entity.yPrev, entity.y, entity.progress01);

    
    RenderSprite_Grid(sprite, &lvlData, renderer, &data->camera, xAnimated, yAnimated);
  }
}
