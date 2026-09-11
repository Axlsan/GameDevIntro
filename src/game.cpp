#include "game.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_scancode.h"
#include "entity.h"
#include "levelRenderer.h"
#include <cstddef>
#include <cstdio>

extern "C" {
  void Initialize(GameData* data, SDL_Renderer* renderer){
    printf("dll sizeof(GameData) = %zu\n", sizeof(GameData));
    data->ground = AssetManagement::LoadSprite(data->arenaImages, renderer, "ground.png");
    data->wall = AssetManagement::LoadSprite(data->arenaImages, renderer, "wall.png");
    data->player = AssetManagement::LoadSprite(data->arenaImages, renderer, "player.png");

    data->currentLevelIndex = 0;
    CreateLevel(data->arenaLevels, &data->levels[0], "assets/levels/map.tmj");
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

      }
    }
/*
    if(keys[SDL_SCANCODE_RIGHT]){
      data->rect.x += data->moveSpeed * dt;
    }

    if(keys[SDL_SCANCODE_LEFT]){
      data->rect.x -= data->moveSpeed * dt;
    }

    if(keys[SDL_SCANCODE_UP]){
      data->rect.y -= data->moveSpeed * dt;
      }
  
    if(keys[SDL_SCANCODE_DOWN]){
      data->rect.y += data->moveSpeed * dt;
    }*/

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
