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

struct GameData {
  Sprite* fallback;
  Sprite* wall;
  Sprite* ground;
  Sprite* player;
  Sprite* box;
  Memory::Arena* arenaLevels;
  Memory::Arena* arenaEntities;
  Memory::Arena* arenaImages;

  LevelData* levels;
  int levelCount;
  int currentLevelIndex;

  float moveSpeed;

  //bool* keysPrevious;

  const float* dt;

  ImGuiContext* ImGUIContext;

  uint32_t commandTimestamp;
  
  Position* inputBuffer;
  int inputBufferCapacity;
  int inputBufferWriteCount;
  int inputBufferReadCount;
  
  LevelData* GetCurrentLevel(){
    return &levels[currentLevelIndex];
  }
  
  Memory::Arena* arenaCommands;
  CommandBuffer* commandBuffer;


  Input input;
  Arena* arenaInput;

  Camera camera;

  Sprite* spriteBuffer;

  bool editLevel;
  Editor editorData;
};

