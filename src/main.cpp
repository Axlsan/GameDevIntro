#include <sdkddkver.h>
#include <windows.h>
#include <cstddef>
#include <timeapi.h>
#include <fileapi.h>
#include <cstdio>
#include <fstream>

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_timer.h"
#include "common.h"
#include "arena.h"
#include "gameState.h"
#include "levels.h"

SDL_Window* window;
SDL_Renderer* renderer;

Uint64 NOW = 0;
Uint64 PREV = 0;

constexpr const char* NAME_OF_DLL = "apa_game.dll";
constexpr const char* NAME_OF_TEMP_DLL = "apa_temp.dll";

typedef void (*Function_Initialize) (GameData* data, SDL_Renderer* renderer);
typedef bool (*Function_HandleEvents) (GameData* data, SDL_Event event);
typedef void (*Function_Update) (GameData* data, float dt);
typedef void (*Function_Draw) (GameData* data, SDL_Renderer* renderer);
typedef void (*Function_OnQuit) (SDL_Renderer* renderer);

constexpr const char* NAME_OF_FUNC_INIT = "Initialize";
constexpr const char* NAME_OF_FUNC_HANDLE_EVENT = "HandleEvents";
constexpr const char* NAME_OF_FUNC_UPDATE = "Update";
constexpr const char* NAME_OF_FUNC_DRAW = "Draw";
constexpr const char* NAME_OF_FUNC_QUIT = "OnQuit";

struct DLL_INFO{
  HMODULE dll;
  FILETIME timestamp;
  Function_Initialize initialize;
  Function_HandleEvents handleEvents;
  Function_Update update;
  Function_Draw draw;
  Function_OnQuit quit;
};

FILETIME GetTimestamp(){
  WIN32_FIND_DATA data;
  HANDLE handle = FindFirstFile(NAME_OF_DLL, &data);
  FILETIME time_of_last_change = data.ftLastWriteTime;
  FindClose(handle);
  return time_of_last_change;
}

void CalculateRemainingFrameTime_MS(double* milliseconds){
    Uint64 frame_end_time_ns = SDL_GetTicksNS();
    double frame_time_spent_ns = frame_end_time_ns - PREV;
    double frame_time_spent_ms = frame_time_spent_ns / 1e6;
    *milliseconds  = FRAME_TIME_MS - frame_time_spent_ms;
}

bool LoadDLL(DLL_INFO* info, int depth = 0){
  printf("loading dll");
  if(depth > 20){
    printf("failed to write temp DLL");
    return false;
  }
  bool success = CopyFile(NAME_OF_DLL, NAME_OF_TEMP_DLL, false);
  if(!success){
    Sleep(50);
    return LoadDLL(info, depth + 1);
  }
  info->dll = LoadLibrary(NAME_OF_TEMP_DLL);
  if(info->dll == nullptr){
    printf("could not load dll");
    return false;
  }
  info->initialize = (Function_Initialize)GetProcAddress(info->dll, NAME_OF_FUNC_INIT);
  info->handleEvents = (Function_HandleEvents)GetProcAddress(info->dll, NAME_OF_FUNC_HANDLE_EVENT);
  info->update = (Function_Update)GetProcAddress(info->dll, NAME_OF_FUNC_UPDATE);
  info->draw = (Function_Draw)GetProcAddress(info->dll, NAME_OF_FUNC_DRAW);
  info->quit = (Function_OnQuit)GetProcAddress(info->dll, NAME_OF_FUNC_QUIT);
  info->timestamp = GetTimestamp();
  return true;
}

void UnloadDLL(DLL_INFO* info){
  FreeLibrary(info->dll);
  info->dll = nullptr;
  DeleteFile(NAME_OF_TEMP_DLL);
}

void* AllocateGameMemory(){
  void* blob = malloc(GAME_MEMORY_ALLOWANCE);
  if(blob == nullptr){
    printf("fatal error: could not allocate memory");
    return nullptr;
  }
  printf("memory succesfully allocated");
  return blob;
}

void SDL_Setup(){
  SDL_Init(SDL_INIT_EVENTS);
  window = SDL_CreateWindow("pilot", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  renderer = SDL_CreateRenderer(window, NULL);
}

void CalculateDeltaTime(float& dt){
  NOW = SDL_GetTicksNS();
  dt = NOW - PREV;
  dt = SDL_NS_TO_SECONDS(dt);
  PREV = NOW;
}

void DLL_CheckStatus(DLL_INFO* dll){
  FILETIME timestamp = GetTimestamp();
  bool is_timestamp_changed = CompareFileTime(&dll->timestamp, &timestamp) != 0;
  if(is_timestamp_changed){
    UnloadDLL(dll);
    LoadDLL(dll);
  }
}

// Save & Load functions
void StoreGameState(Memory::Arena* arena){
  std::ofstream file("temp_state.bin", std::ios::binary);
  file.write(reinterpret_cast<const char*>(arena->base), arena->size);
  file.close();
  printf("Saved successfully \n");
}

void RetrieveGameState(Memory::Arena* arena){
  std::ifstream file("temp_state.bin", std::ios::binary);
  if(file.is_open()){
    file.read(reinterpret_cast<char*>(arena->base), arena->size);
    file.close();
    printf("Loaded successfully \n");
  }
  else printf("Load failed \n");
}


int main() {
  void* game_memory = AllocateGameMemory();
  if(game_memory == nullptr){
    return 1;
  }

  // Memory allocation
  Memory::Arena* arenaMain = new Memory::Arena();
  Memory::Initialize(arenaMain, game_memory, GAME_MEMORY_ALLOWANCE);
  GameData* gameData = (GameData*)Memory::Allocate(arenaMain, sizeof(GameData));


  size_t IMAGE_ARENA_SIZE = sizeof(Image) * 1024;

  gameData->arenaImages = Memory::CreateSubArena(arenaMain, IMAGE_ARENA_SIZE);
  gameData->arenaLevels = Memory::CreateSubArena(arenaMain, MEGABYTES(3));
  gameData->arenaEntities = Memory::CreateSubArena(gameData->arenaLevels, MEGABYTES(1));

  // Allocate the pointer (array) of levels
  gameData->levels = (LevelData*)Memory::Allocate(gameData->arenaLevels, sizeof(LevelData));


  gameData->keysPrevious = (bool*)Memory::Allocate(gameData->arenaLevels, sizeof(bool) * SDL_SCANCODE_COUNT);

  SDL_Setup();


  gameData->fallback = AssetManagement::LoadSprite(gameData->arenaImages, renderer, "dog.png");
  if(gameData->fallback == nullptr){
    return 1;
  }


  MMRESULT result = timeBeginPeriod(1);
  if(result == TIMERR_NOCANDO){
    printf("Could not increase their resolution");
    Sleep(3000);
    return 3;
  }

  printf("exe sizeof(GameData) = %zu\n", sizeof(GameData));
  DLL_INFO dll;
  bool dll_successfully_loaded = LoadDLL(&dll);

  if(dll_successfully_loaded == false){
    return 2;
  }

  dll.initialize(gameData, renderer);
  bool running = true;
  float dt;


  while(running){
    DLL_CheckStatus(&dll);
    CalculateDeltaTime(dt);
    SDL_Event event;

    while(SDL_PollEvent(&event)){
      running = dll.handleEvents(gameData, event);
      if(running == false){
        break;
      }

      // Save & Load keys
      if(event.type == SDL_EVENT_KEY_DOWN){
        if(event.key.key == SDLK_F9) StoreGameState(arenaMain);
        if(event.key.key == SDLK_F10) RetrieveGameState(arenaMain);
      }
    }
    dll.update(gameData, dt);
    dll.draw(gameData, renderer);

    double time_to_sleep_ms;
    CalculateRemainingFrameTime_MS(&time_to_sleep_ms);

    if(time_to_sleep_ms > 0.0){
      SDL_Delay(time_to_sleep_ms);
    }
    else{
      printf("Missed frame \n");
    }
  }
  dll.quit(renderer);
  SDL_Quit();
  return 0;
}
