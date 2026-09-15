#pragma once
#include "SDL3/SDL_rect.h"
#include "arena.h"
#include "command.h"
#include "image.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "levels.h"

struct GameData {
  Image* fallback;
  Image* wall;
  Image* ground;
  Image* player;
  Image* box;
  Memory::Arena* arenaLevels;
  Memory::Arena* arenaEntities;
  Memory::Arena* arenaImages;

  LevelData* levels;
  int levelCount;
  int currentLevelIndex;

  float moveSpeed;

  bool* keysPrevious;

  const float* dt;

  ImGuiContext* ImGUIContext;
  
  LevelData* GetCurrentLevel(){
    return &levels[currentLevelIndex];
  }
  
  Memory::Arena* arenaCommands;
  CommandBuffer* commandBuffer;
};

