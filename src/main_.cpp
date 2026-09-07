#include <iostream>
#include <windows.h>
using namespace std;

struct MemoryArena{
  unsigned char* base;
  size_t size;
  size_t used;
};

void* Allocate(MemoryArena* arena, size_t size){
  if(arena->used + size > arena->size) return nullptr;

  void* latestPoint = arena->base + arena->used;
  arena->used += size;
  return latestPoint;
}

void ArenaInitialize(MemoryArena* arena, void* start, size_t size){
  arena->base = (unsigned char*)start;
  arena->size = size;
  arena->used = 0;
}

void ArenaReset(MemoryArena* arena){
  arena->used = 0;
}
struct Character{
  enum CHARACTER_TYPE {HERO, ENEMY};
  int health;
  int damage;
  bool isAlive;
  char* name;
};

struct GameData{
  Character* characters;
  int characterCount;
  int score;
};

GameData* gameData;
MemoryArena arena;

int main(){
  
  cout << "apa" << endl;
  Sleep(2000);
  
  size_t memorySize = (1024 * 1024 * 4);
  void* blobOfMemory = malloc(memorySize);

  if(blobOfMemory == nullptr) return 1;


  ArenaInitialize(&arena, blobOfMemory, memorySize);
  
  cout << "bepa" << endl;
  Sleep(2000);
  gameData = (GameData*)Allocate(&arena, sizeof(GameData));
  gameData->characterCount = 10;
  gameData->score = 0;

  
  cout << "cepa" << endl;
  Sleep(2000);
  
  void* charactersStartPoint = Allocate(&arena, sizeof(Character) * gameData->characterCount);
  gameData->characters = (Character*)charactersStartPoint;

  gameData->characters[3].health = 32;

  cout << gameData->characters[3].health << endl;

  Sleep(2000);

  return 0;
}
