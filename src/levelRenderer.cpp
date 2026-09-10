#include <cstdint>
#include <iterator>

#include "levelRenderer.h"
#include "common.h"
#include "rendering.h"

void RenderLevel(GameData* gameData, SDL_Renderer* renderer){

  LevelData lvl = gameData->levels[gameData->currentLevel];

  int boardWidthPxHalf = lvl.w * CELL_SIZE_PX / 2;
  int boardHeightPxHalf = lvl.h * CELL_SIZE_PX / 2;

  for(int x = 0; x < lvl.w; x++){
    for(int y = 0; y < lvl.h; y++){
      uint8_t cellType = lvl.getCellID(x, y);

      Image* sprite;
      switch (cellType) {
        case 0:
          continue;
        case 4:
          sprite = gameData->ground;
          break;
        case 5:
          sprite = gameData->wall;
          break;
        default:
          sprite = gameData->fallback;
          break;
      }

      float xPos = x * CELL_SIZE_PX;
      float yPos = y * CELL_SIZE_PX;

      xPos += SCREEN_WIDTH / 2;
      yPos += SCREEN_HEIGHT / 2;

      xPos -= boardWidthPxHalf;
      yPos -= boardHeightPxHalf;

      RenderSprite(sprite, renderer, xPos, yPos);
    }
  }
}

void RenderEntities(GameData* data, SDL_Renderer* renderer){
  LevelData lvlData = data->levels[data->currentLevel];
  for(int i = 0; i < lvlData.entityCount; i++){
    Image* img;
    Entity entity = lvlData.entityBuffer[i];
    switch(entity.id){
      case 6:
        img = data->player;
        break;
      default:
        img = data->fallback;
        break;
    }

    int xPos = 0;
    int yPos = 0;

    xPos += SCREEN_WIDTH / 2;
    yPos += SCREEN_HEIGHT / 2;

    xPos -= data->levels[data->currentLevel].w * CELL_SIZE_PX / 2;
    yPos -= data->levels[data->currentLevel].h * CELL_SIZE_PX / 2;

    xPos += entity.x * CELL_SIZE_PX;
    yPos += entity.y * CELL_SIZE_PX;

    RenderSprite(img, renderer, xPos, yPos);
  }
}
