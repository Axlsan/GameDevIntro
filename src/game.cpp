#include "game.h"
#include "SDL3/SDL_blendmode.h"
#include "SDL3/SDL_render.h"
#include "arena.h"
#include "common.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_scancode.h"
#include "command.h"
#include "dev_gui.h"
#include "entity.h"
#include "gameState.h"
#include "imgui/imgui_impl_sdlrenderer3.h"
#include "levelEditor.h"
#include "levelRenderer.h"
#include "imgui/imgui.h"
#include "input.h"
#include "levels.h"
#include "rendering.h"
#include "spriteLibrary.h"

#include <cassert>
#include <cstddef>
#include <cstdio>

extern "C" {

  
  void InitializeGame(Gameplay* gameplay, Arena* arenaLevels){
    assert(gameplay->initialized == false);
    gameplay->currentLevelIndex = 1;
    CreateLevel(arenaLevels, &gameplay->levels[0], "assets/levels/map.tmj");
    CreateLevel(arenaLevels, &gameplay->levels[1], "assets/levels/map_box.tmj");
    gameplay->initialized = true;
  }

  void UpdateTitleScreen(TitleScreen* titleScreen, const float dt){
  
  }

  void UpdateGame(Gameplay* gameplay, Input* input, const float dt){
  
  }

  
  void StartLevel(Gameplay* gameplay, Arena* arenaCommands, Arena* arenaEntities){
    Reset(arenaCommands);
    CreateEntities(&gameplay->levels[gameplay->currentLevelIndex], arenaEntities);
  }
  
  void ChangeScene(GameData* data, SCENE_TYPES newScene){
    assert(newScene != data->sceneCurrent);

    data->scenePrevious = data->sceneCurrent;
    data->sceneCurrent = newScene;
    data->transition.state = data->scenePrevious == SCENE_TYPES::NONE ? Transition::FadeFrom : Transition::FadeTo;
    data->transition.fadeTimeElapsed = 0;

    switch(data->sceneCurrent){
      case SCENE_TYPES::TITLESCREEN:
        data->transition.fadeTimeDuration = 1;
        break;
      case SCENE_TYPES::MAINMENU:
        break;
      case SCENE_TYPES::GAME:{
        data->transition.fadeTimeDuration = 0.5f;
        Gameplay* gameplay = &data->scenes.gameplay;
        assert(gameplay->initialized);
        StartLevel(gameplay, data->arenaCommands, data->arenaEntities);
        break;
      }
      case SCENE_TYPES::CREDITS:
        break;
      case SCENE_TYPES::NONE:
        assert(false);
    }
  }

  
  void Initialize(GameData* data, SDL_Window* window, SDL_Renderer* renderer){
//    printf("dll sizeof(GameData) = %zu\n", sizeof(GameData));
    DEV::Initialize(data, window, renderer);
    AssetManagement::LoadAllSprites(data->spriteBuffer, renderer);
    data->ImGUIContext = ImGui::GetCurrentContext();

    SDL_Texture* blackFade = GetSprite(SPRITE_ID::black_1x1,data->spriteBuffer)->texture;
    SDL_SetTextureBlendMode(blackFade, SDL_BLENDMODE_BLEND);
    InitializeGame(&data->scenes.gameplay, data->arenaLevels);
    ChangeScene(data, SCENE_TYPES::GAME);
    //CreateEntities(&data->levels[data->currentLevelIndex], data->arenaEntities);
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

  
  bool TryMove(Entity* mover, LevelData* level, CommandBuffer* cmdBuffer, int xDir, int yDir, int strength){
    if(!HasBehaviour(mover, CAN_MOVE)){
      return false;
    }

    if(strength < 0){
      return false;
    }

    int testX = mover->x + xDir;
    int testY = mover->y + yDir;
    
    Entity* stepIntoEntity = GetEntity(level, testX, testY);
    ID stepIntoTileID = (ID)getCellID(level, testX, testY);

    if(stepIntoEntity == nullptr){
      if(stepIntoTileID == ID::GROUND){
        MoveCommand mv(mover, xDir, yDir);
        mv.type = CMD_TYPE::MOVE;
        mv.entity = mover;
        mv.xDir = xDir;
        mv.yDir = yDir;
        Push(cmdBuffer, mv, level);
        return true;
      }
      return false;
    }
    
    if(HasBehaviour(stepIntoEntity, CAN_MOVE) && !HasBehaviour(stepIntoEntity, UNPUSHABLE)){
      if (TryMove(stepIntoEntity, level, cmdBuffer, xDir, yDir, --strength)) {
        MoveCommand mv(mover, xDir, yDir);
        AddBehaviour(mover, Behaviour::IS_PUSHING);
        Push(cmdBuffer, mv, level);
        return true;
      }
    }
    return false;
  }
  
  void Update(GameData* data,float dt){
    
    Gameplay* gameplay = &data->scenes.gameplay;
    TitleScreen* titleScreen = &data->scenes.titleScreen;
    EditorData* editorData = &data->editorData;
    Transition* transition = &data->transition;
    
    if(transition->state != Transition::Inactive){
      transition->fadeTimeElapsed += dt;
      if(transition->fadeTimeElapsed >= transition->fadeTimeDuration){
        switch (transition->state) {
          case Transition::Inactive:
            break;
          case Transition::FadeTo:
            transition->state = Transition::FadeFrom;
            break;
          case Transition::FadeFrom:
            transition->state = Transition::Inactive;
            break;
        }
      }
    }

    switch (data->sceneCurrent) {
      case SCENE_TYPES::TITLESCREEN:
        UpdateTitleScreen(titleScreen, dt);
        if(AnyKeyPressed(&data->input)){
          if(transition->state == Transition::FadeTo || transition->state == Transition::Inactive){
            ChangeScene(data, SCENE_TYPES::GAME);
          }
        }
        break;

      case SCENE_TYPES::MAINMENU:
        break;

      case SCENE_TYPES::GAME:
        UpdateGame(gameplay, &data->input, dt);
        break;
      case SCENE_TYPES::CREDITS:
        break;
      case SCENE_TYPES::NONE:
        assert(false);
    }
    
    //Level edit
    if(KeyPressed(&data->input, SDL_SCANCODE_F2)){
      editorData->editLevel = !editorData->editLevel;
    }
    if(editorData->editLevel){
      EDITOR::Update(&editorData->editor, &data->input, GetCurrentLevel(gameplay), gameplay->commandBuffer);
    }

    if(KeyPressed(&data->input, SDL_SCANCODE_5)){
      ChangeScene(data, SCENE_TYPES::TITLESCREEN);
      return;
    }


    // UNDO/REDO
    if(KeyPressed(&data->input, SDL_SCANCODE_Z) || KeyHeldForTime(&data->input, SDL_SCANCODE_Z, UNDO_REPEAT_TIME)){
      ResetKeyHeldTime(&data->input, SDL_SCANCODE_Z);
      if(KeyHeld(&data->input, SDL_SCANCODE_LSHIFT)){
        Redo(gameplay->commandBuffer, GetCurrentLevel(gameplay));
      }
      else{
        Undo(gameplay->commandBuffer, GetCurrentLevel(gameplay));
      }
    }

    if(KeyPressed(&data->input, SDL_SCANCODE_RIGHT) || KeyHeldForTime(&data->input, SDL_SCANCODE_RIGHT, (1 / MOVE_SPEED) * 1.15)){
      gameplay->inputBuffer[gameplay->inputBufferWriteCount++ % gameplay->inputBufferCapacity] =
        {1,0};
    }
    
    else if(KeyPressed(&data->input, SDL_SCANCODE_LEFT) || KeyHeldForTime(&data->input, SDL_SCANCODE_LEFT, (1 / MOVE_SPEED) * 1.15)){
      gameplay->inputBuffer[gameplay->inputBufferWriteCount++ % gameplay->inputBufferCapacity] =
        {-1,0};
    }

    else if(KeyPressed(&data->input, SDL_SCANCODE_UP) || KeyHeldForTime(&data->input, SDL_SCANCODE_UP, (1 / MOVE_SPEED) * 1.15)){
      gameplay->inputBuffer[gameplay->inputBufferWriteCount++ % gameplay->inputBufferCapacity] =
        {0,-1};
    }

    else if(KeyPressed(&data->input, SDL_SCANCODE_DOWN) || KeyHeldForTime(&data->input, SDL_SCANCODE_DOWN, (1 / MOVE_SPEED) * 1.15)){
      gameplay->inputBuffer[gameplay->inputBufferWriteCount++ % gameplay->inputBufferCapacity] =
        {0,1};
    }

    bool areEntitiesMoving = false;
    for(int i = 0; i < GetCurrentLevel(gameplay)->entityCount; i++){
      Entity* entity = &GetCurrentLevel(gameplay)->entityBuffer[i];
      if(HasBehaviour(entity, CAN_MOVE) && IsMoving(entity)){
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
      if(gameplay->inputBufferReadCount == gameplay->inputBufferWriteCount){
        return;
      }

      gameplay->commandBuffer->timestamp++;

      for(int i = 0; i < GetCurrentLevel(gameplay)->entityCount; i++){
        Entity* entity = &GetCurrentLevel(gameplay)->entityBuffer[i];
        if(HasBehaviour(entity, Behaviour::IS_PUSHING)){
          RemoveBehaviour(entity, Behaviour::IS_PUSHING);
        }
        
        if(HasBehaviour(entity, (Behaviour)(RESPOND_TO_INPUT | CAN_MOVE))){
          if(HasBehaviour(entity, Behaviour::IS_PETRIFIED)){
            continue;
          }
          int xDir = gameplay->inputBuffer[gameplay->inputBufferReadCount % gameplay->inputBufferCapacity].x;
          int yDir = gameplay->inputBuffer[gameplay->inputBufferReadCount % gameplay->inputBufferCapacity].y;

          Direction newFacing = DirectionFromXY(xDir, yDir);
          if(newFacing != entity->facing){
            RotateCommand rotate(entity, entity->facing, newFacing);
          }
          TryMove(entity, GetCurrentLevel(gameplay), gameplay->commandBuffer, xDir, yDir, entity->strength);
        }
      }
      gameplay->inputBufferReadCount++;
    }
  }

  void DrawScene(GameData* data, SCENE_TYPES scene, SDL_Renderer* renderer){
    switch (scene) {
      case SCENE_TYPES::TITLESCREEN:{
        Sprite* background = GetSprite(SPRITE_ID::titleScreenBackground, data->spriteBuffer);
        break;
      }

      case SCENE_TYPES::MAINMENU:
        break;

      case SCENE_TYPES::GAME:
        RenderLevel(data, renderer);
        RenderEntities(data, renderer);
        break;

      case SCENE_TYPES::CREDITS:
        break;

      case SCENE_TYPES::NONE:
        assert(false);
    }
  }

  void Draw(GameData* data, SDL_Renderer* renderer){
    
    
    DEV::PreDraw(data->ImGUIContext);

    SDL_SetRenderDrawColor(renderer, 80, 50, 80, 255);
    SDL_RenderClear(renderer);


    switch(data->transition.state){
      case Transition::Inactive:
        DrawScene(data, data->sceneCurrent, renderer);
        break;
      
      case Transition::FadeTo:{
        DrawScene(data, data->scenePrevious, renderer);
        float alpha = data->transition.fadeTimeElapsed / data->transition.fadeTimeDuration;
        RenderSprite_World(GetSprite(SPRITE_ID::black_1x1, data->spriteBuffer), renderer, &data->camera, 0, 0, SCREEN_WIDTH, alpha);
        break;
      }

      case Transition::FadeFrom:{
        DrawScene(data, data->sceneCurrent, renderer);
        float alpha = 1 - data->transition.fadeTimeElapsed / data->transition.fadeTimeDuration;
        RenderSprite_World(GetSprite(SPRITE_ID::black_1x1, data->spriteBuffer), renderer, &data->camera, 0, 0, SCREEN_WIDTH, alpha);
        break;
      }
    }

    DEV::Draw(data, renderer);
    SDL_RenderPresent(renderer);
  }

  void OnQuit(SDL_Renderer* renderer){
    SDL_DestroyRenderer(renderer);
  }
}

