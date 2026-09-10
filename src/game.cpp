#include "game.h"
#include "levelRenderer.h"
#include <cstddef>
#include <cstdio>

extern "C" {
  void Initialize(GameData* data, SDL_Renderer* renderer){
    printf("dll sizeof(GameData) = %zu\n", sizeof(GameData));
    data->ground = AssetManagement::LoadSprite(data->arenaImages, renderer, "ground.png");
    data->wall = AssetManagement::LoadSprite(data->arenaImages, renderer, "wall.png");
    data->player = AssetManagement::LoadSprite(data->arenaImages, renderer, "player.png");

    data->currentLevel = 0;
    CreateLevel(data->arenaLevels, &data->levels[0], "assets/levels/map.tmj");
    CreateEntities(&data->levels[data->currentLevel], data->arenaEntities);
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
  
  void Update(GameData* data,float dt){
    const bool* keys = SDL_GetKeyboardState(NULL);
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
