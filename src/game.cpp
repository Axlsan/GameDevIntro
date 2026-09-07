#include "game.h"
#include <cstddef>

extern "C" {
  void Initialize(GameData* data){
    data->rect.x = 100;
    data->rect.y = 100;
    data->rect.h = 50;
    data->rect.w = 50;
    data->move_speed = 100;
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

    if(keys[SDL_SCANCODE_RIGHT]){
      data->rect.x += data->move_speed * dt;
    }

    if(keys[SDL_SCANCODE_LEFT]){
      data->rect.x -= data->move_speed * dt;
    }

    if(keys[SDL_SCANCODE_UP]){
      data->rect.y -= data->move_speed * dt;
      }
  
    if(keys[SDL_SCANCODE_DOWN]){
      data->rect.y += data->move_speed * dt;
    }
  }
  void Draw(GameData* data, SDL_Renderer* renderer){
    SDL_SetRenderDrawColor(renderer, 80, 50, 80, 255);
    SDL_RenderClear(renderer);

    SDL_RenderTexture(renderer, data->fallback->texture, NULL, &data->rect);

    SDL_RenderPresent(renderer);
  }

  void OnQuit(SDL_Renderer* renderer){
    SDL_DestroyRenderer(renderer);
  }
}
