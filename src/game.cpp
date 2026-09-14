#include "game.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_scancode.h"
#include "entity.h"
#include "levelRenderer.h"
#include <cstddef>
#include <cstdio>

extern "C" {
  void Initialize(GameData* data, SDL_Renderer* renderer){
//    printf("dll sizeof(GameData) = %zu\n", sizeof(GameData));
    
    data->ground = AssetManagement::LoadSprite(data->arenaImages, renderer, "ground.png");
    data->wall = AssetManagement::LoadSprite(data->arenaImages, renderer, "wall.png");
    data->player = AssetManagement::LoadSprite(data->arenaImages, renderer, "player.png");
    data->box = AssetManagement::LoadSprite(data->arenaImages, renderer, "box.png");

    data->currentLevelIndex = 1;
    CreateLevel(data->arenaLevels, &data->levels[0], "assets/levels/map.tmj");
    CreateLevel(data->arenaLevels, &data->levels[1], "assets/levels/map_box.tmj");
    CreateEntities(&data->levels[data->currentLevelIndex], data->arenaEntities);
  }

  bool HandleEvents(GameData *data, SDL_Event event){
    if(event.type != SDL_EVENT_KEY_DOWN){
      return true;
    }

    if(event.key.key == SDLK_ESCAPE){
      return false;
    }
      return true;
  }
  
  bool KeyPressed(SDL_Scancode key, const bool* current, const bool* previous){
    if(previous == nullptr){
      return current[key];
    }
    return current[key] && !previous[key];
  }

  bool KeyHeld(SDL_Scancode key, const bool *current, const bool *previous){
    if(previous == nullptr){
      return false;
    }
    return current[key] && previous[key];
  }

  bool KeyReleased(SDL_Scancode key, const bool *current, const bool *previous){
    if(previous == nullptr){
      return false;
    }
    return !current[key] && previous[key];
  }

  bool TryMove(Entity *mover, LevelData *level, int xDir, int yDir){
    if(mover->HasBehaviour(CAN_MOVE) == false){
      return false;
    }

    int testX = mover->x + xDir;
    int testY = mover->y + yDir;
    Entity* stepIntoEntity = level->GetEntity(testX, testY);
    ID stepIntoTileID = (ID)level->getCellID(testX, testY);

    if(stepIntoEntity == nullptr){
      if(stepIntoTileID == ID::GROUND){
        mover->x = testX;
        mover->y = testY;
        return true;
      }
      return false;
    }
    
    if(stepIntoEntity->HasBehaviour(CAN_MOVE)){
      if(TryMove(stepIntoEntity, level, xDir, yDir)){
        mover->x = testX;
        mover->y = testY;
        return true;
      }
    }
    return false;
  }
  
  void Update(GameData* data,float dt){
//    const bool* keys = SDL_GetKeyboardState(NULL);

    const bool* keys = SDL_GetKeyboardState(nullptr);
    
    for(int i = 0; i < data->GetCurrentLevel()->entityCount; i++){
      Entity* entity = &data->GetCurrentLevel()->entityBuffer[i];

      if(entity->HasBehaviour((Behaviour)(Behaviour::RESPOND_TO_INPUT | Behaviour::CAN_MOVE))){
        int xChange = 0;
        int yChange = 0;

        if(KeyPressed(SDL_SCANCODE_RIGHT, keys, data->keysPrevious)){
          xChange = 1;
        }

        else if(KeyPressed(SDL_SCANCODE_LEFT, keys, data->keysPrevious)){
          xChange = -1;
        }

        else if(KeyPressed(SDL_SCANCODE_UP, keys, data->keysPrevious)){
          yChange = -1;
        }

        else if(KeyPressed(SDL_SCANCODE_DOWN, keys, data->keysPrevious)){
          yChange = 1;
        }

        if(xChange != 0 || yChange != 0){
          TryMove(entity, data->GetCurrentLevel(), xChange, yChange);
        }

        
/*
        if(xChange != 0|| yChange !=0){
          int stepIntoX = entity->x + xChange;
          int stepIntoY = entity->y + yChange;
          
          Entity* stepIntoEntity = data->GetCurrentLevel()->GetEntity(stepIntoX, stepIntoY);
          uint8_t stepIntoTileID = data->GetCurrentLevel()->getCellID(stepIntoX, stepIntoY);

          if(stepIntoEntity == nullptr){
            if(stepIntoTileID == (uint8_t)ID::GROUND){
              entity->x = stepIntoX;
              entity->y = stepIntoY;
            }
          }
        }
        */
      }
    }

    // copy keys to keysPrevious
    memcpy((void*)data->keysPrevious, keys, SDL_SCANCODE_COUNT * sizeof(bool));
  }
  void Draw(GameData* data, SDL_Renderer* renderer){
    SDL_SetRenderDrawColor(renderer, 80, 50, 80, 255);
    SDL_RenderClear(renderer);

    RenderLevel(data, renderer);
    RenderEntities(data, renderer);
    
    SDL_RenderPresent(renderer);
  }

  void OnQuit(SDL_Renderer* renderer){
    SDL_DestroyRenderer(renderer);
  }
}
