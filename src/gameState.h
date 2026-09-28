#pragma once
#include "SDL3/SDL_rect.h"
#include "arena.h"
#include "camera.h"
#include "command.h"
#include "levelEditor.h"
#include "spriteLibrary.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "levels.h"
#include "input.h"

struct Gameplay{
  CommandBuffer* commandBuffer;
  LevelData* levels;
  int levelCount;
  int currentLevelIndex;
  Position* inputBuffer;
  int inputBufferCapacity;
  int inputBufferWriteCount;
  int inputBufferReadCount;
  bool initialized;
};

struct MainMenu{
  
};

struct TitleScreen{
  
};

struct Credits{
  
};

enum class SCENE_TYPES : uint8_t{
  NONE,
  TITLESCREEN,
  MAINMENU,
  GAME,
  CREDITS
};



struct Scenes{
  Gameplay gameplay;
  MainMenu mainMenu;
  TitleScreen titleScreen;
  Credits credits;
};

struct Transition{
 enum States{
   Inactive,
   FadeTo,
   FadeFrom
 };
 States state;
 float fadeTimeElapsed;
 float fadeTimeDuration = 1;
};

struct EditorData{
  bool editLevel;
  Editor editor;
  // CommandBuffer* commandBuffer;
};

struct GameData {

  SCENE_TYPES sceneCurrent;
  SCENE_TYPES scenePrevious;
  Scenes scenes;
  Transition transition;
  EditorData editorData;
  
  Sprite* fallback;
  Sprite* wall;
  Sprite* ground;
  Sprite* player;
  Sprite* box;
  Memory::Arena* arenaLevels;
  Memory::Arena* arenaEntities;
  Memory::Arena* arenaImages;

  //LevelData* levels;
  //int levelCount;
  //int currentLevelIndex;

  float moveSpeed;


  const float* dt;

  ImGuiContext* ImGUIContext;

  
  //Position* inputBuffer;
  //int inputBufferCapacity;
  //int inputBufferWriteCount;
  //int inputBufferReadCount;
  
  //LevelData* GetCurrentLevel(){
  //  return &levels[currentLevelIndex];
  //}
  
  Memory::Arena* arenaCommands;
  // CommandBuffer* commandBuffer;


  Input input;
  Arena* arenaInput;

  Camera camera;

  Sprite* spriteBuffer;

  // bool editLevel;
  // Editor editor;

  Memory::Arena* arenaScratch;

  Memory::Arena* arenaMain;
};

inline LevelData* GetCurrentLevel(Gameplay* game){
  return &game->levels[game->currentLevelIndex];
}
