#include "game.h"
#include "common.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_scancode.h"
#include "command.h"
#include "dev_gui.h"
#include "entity.h"
#include "gameState.h"
#include "imgui/imgui_impl_sdlrenderer3.h"
#include "levelRenderer.h"
#include "imgui/imgui.h"
#include "input.h"

#include <cstddef>
#include <cstdio>

extern "C" {
  void Initialize(GameData* data, SDL_Window* window, SDL_Renderer* renderer){
//    printf("dll sizeof(GameData) = %zu\n", sizeof(GameData));
    DEV::Initialize(window, renderer);
    data->ImGUIContext = ImGui::GetCurrentContext();
    
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
    DEV::ProcessEvents(&event);
    
    if(event.type != SDL_EVENT_KEY_DOWN){
      return true;
    }

    if(event.key.key == SDLK_ESCAPE){
      return false;
    }
      return true;
  }

/*  
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
*/
  
  bool TryMove(Entity* mover, LevelData* level, CommandBuffer* cmdBuffer, int xDir, int yDir, int timestamp){
    if(mover->HasBehaviour(CAN_MOVE) == false){
      return false;
    }

    int testX = mover->x + xDir;
    int testY = mover->y + yDir;
    
    Entity* stepIntoEntity = level->GetEntity(testX, testY);
    ID stepIntoTileID = (ID)level->getCellID(testX, testY);

    if(stepIntoEntity == nullptr){
      if(stepIntoTileID == ID::GROUND){
        MoveCommand mv;
        mv.type = CMD_TYPE::MOVE;
        mv.entity = mover;
        mv.xDir = xDir;
        mv.yDir = yDir;
        Push(cmdBuffer, mv, timestamp);
        return true;
      }
      return false;
    }
    
    if(stepIntoEntity->HasBehaviour(CAN_MOVE)){
      if(TryMove(stepIntoEntity, level, cmdBuffer, xDir, yDir, timestamp)){
        MoveCommand mv;
        mv.type = CMD_TYPE::MOVE;
        mv.entity = mover;
        mv.xDir = xDir;
        mv.yDir = yDir;
        Push(cmdBuffer, mv, timestamp);
        return true;
      }
    }
    return false;
  }
  
  void Update(GameData* data,float dt){
//    const bool* keys = SDL_GetKeyboardState(NULL);

    //const bool* keys = SDL_GetKeyboardState(nullptr);
    

    // UNDO/REDO
    if(KeyPressed(&data->input, SDL_SCANCODE_Z) || KeyHeldForTime(&data->input, SDL_SCANCODE_Z, UNDO_REPEAT_TIME)){
      ResetKeyHeldTime(&data->input, SDL_SCANCODE_Z);
      if(KeyHeld(&data->input, SDL_SCANCODE_LSHIFT)){
        Redo(data->commandBuffer);
      }
      else{
        Undo(data->commandBuffer);
      }
    }

    if(KeyPressed(&data->input, SDL_SCANCODE_RIGHT) || KeyHeldForTime(&data->input, SDL_SCANCODE_RIGHT, (1 / MOVE_SPEED) * 1.15)){
      data->inputBuffer[data->inputBufferWriteCount++ % data->inputBufferCapacity] =
        {1,0};
    }
    
    else if(KeyPressed(&data->input, SDL_SCANCODE_LEFT) || KeyHeldForTime(&data->input, SDL_SCANCODE_LEFT, (1 / MOVE_SPEED) * 1.15)){
      data->inputBuffer[data->inputBufferWriteCount++ % data->inputBufferCapacity] =
        {-1,0};
    }

    else if(KeyPressed(&data->input, SDL_SCANCODE_UP) || KeyHeldForTime(&data->input, SDL_SCANCODE_UP, (1 / MOVE_SPEED) * 1.15)){
      data->inputBuffer[data->inputBufferWriteCount++ % data->inputBufferCapacity] =
        {0,-1};
    }

    else if(KeyPressed(&data->input, SDL_SCANCODE_DOWN) || KeyHeldForTime(&data->input, SDL_SCANCODE_DOWN, (1 / MOVE_SPEED) * 1.15)){
      data->inputBuffer[data->inputBufferWriteCount++ % data->inputBufferCapacity] =
        {0,1};
    }

    bool areEntitiesMoving = false;
    for(int i = 0; i < data->GetCurrentLevel()->entityCount; i++){
      Entity* entity = &data->GetCurrentLevel()->entityBuffer[i];
      if(entity->HasBehaviour(CAN_MOVE) && IsMoving(entity)){
        entity->progress01 += MOVE_SPEED * dt;
        if(entity->progress01 >= 1){
          entity->progress01 = 0;
          entity->xPrev = entity->x;
          entity->yPrev = entity->y;
        }
        if(IsMoving(entity)){
          areEntitiesMoving = true;
        }
      }
    }

    if(!areEntitiesMoving){
      if(data->inputBufferReadCount == data->inputBufferWriteCount){
        return;
      }

      data->commandTimestamp++;

      for(int i = 0; i < data->GetCurrentLevel()->entityCount; i++){
        Entity* entity = &data->GetCurrentLevel()->entityBuffer[i];
        if(entity->HasBehaviour((Behaviour)(RESPOND_TO_INPUT | CAN_MOVE))){
          int xDir = data->inputBuffer[data->inputBufferReadCount % data->inputBufferCapacity].x;
          int yDir = data->inputBuffer[data->inputBufferReadCount % data->inputBufferCapacity].y;
          TryMove(entity, data->GetCurrentLevel(), data->commandBuffer, xDir, yDir, data->commandTimestamp);
        }
      }
      data->inputBufferReadCount++;
    }
    /*
    data->commandTimestamp++;
    
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
          TryMove(entity, data->GetCurrentLevel(), data->commandBuffer, xChange, yChange, data->commandTimestamp);
        
        }


      }
    }*/

  }
  void Draw(GameData* data, SDL_Renderer* renderer){
    
    
    SDL_SetRenderDrawColor(renderer, 80, 50, 80, 255);
    SDL_RenderClear(renderer);

    RenderLevel(data, renderer);
    RenderEntities(data, renderer);


    DEV::PreDraw(data->ImGUIContext);
    DEV::Draw(data, renderer);
    SDL_RenderPresent(renderer);
  }

  void OnQuit(SDL_Renderer* renderer){
    SDL_DestroyRenderer(renderer);
  }
}
