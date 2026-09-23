#include <cstdint>
#include <iterator>
#include <cmath>
#include <numbers>

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
      uint8_t cellType = getCellID(&lvlData, x, y);

      //Sprite* sprite = GetSpriteFromID((ID)cellType, gameData->spriteBuffer);
      Sprite* sprite;

      if(ID(cellType) == ID::GROUND){
        sprite = & gameData->spriteBuffer[(x + y) % 2 == 0 ? (int)SPRITE_ID::Ground : (int)SPRITE_ID::GroundAlt];
      }
      else{
        sprite = GetSpriteFromID((ID)cellType, gameData->spriteBuffer);
      }

      
      if(sprite == nullptr) continue;
      RenderSprite_Grid(sprite, &lvlData, renderer, &gameData->camera, x, y);
    }
  }
}

void RenderEntities(GameData* data, SDL_Renderer* renderer){
  LevelData lvlData = data->levels[data->currentLevelIndex];

    for(int i = 0; i < lvlData.entityCount; i++){
    Entity entity = lvlData.entityBuffer[i];
    if(entity.id == ID::NONE){
      continue;
    }

    Sprite* sprite = GetSpriteFromID(entity.id, data->spriteBuffer);
    if(HasBehaviour(&entity, Behaviour::IS_PETRIFIED)){
      sprite = GetSpriteFromID(ID::ROCK, data->spriteBuffer);
    }
    
    // Animation
    float xAnimated = std::lerp(entity.xPrev, entity.x, entity.progress01);
    float yAnimated = std::lerp(entity.yPrev, entity.y, entity.progress01);

    float dropshadowY = yAnimated;

    if(HasBehaviour(&entity, Behaviour::JUMPS) && !HasBehaviour(&entity, Behaviour::IS_PUSHING)){
      yAnimated -= 0.5 * sinf(entity.progress01 * std::numbers::pi);
    }

    Sprite* dropshadow = &data->spriteBuffer[(int)SPRITE_ID::DropShadow];

    RenderEntity_OnTile(dropshadow, &lvlData, renderer, &data->camera, xAnimated, dropshadowY, 1, 0.4, false);
    RenderEntity_OnTile(sprite, &lvlData, renderer, &data->camera, xAnimated, yAnimated, 1, 1, entity.facing == Direction::RIGHT);
    
    //RenderSprite_Grid(sprite, &lvlData, renderer, &data->camera, xAnimated, yAnimated);
  }
}
