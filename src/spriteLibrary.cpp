#include <cassert>
#include <string>
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_image.h"
#include "spriteLibrary.h"
#include "SDL3/SDL_surface.h"
#include "arena.h"
using namespace std;

const char* DIRECTORY = "assets/sprites/";
const char* FALLBACK_PATH = "assets/sprites/dog.png";

static const SpriteDataEntry allSpriteData[] = {
  {SPRITE_ID::Fallback,  FALLBACK_PATH},
  {SPRITE_ID::Wall,      "assets/sprites/wall.png"},
  {SPRITE_ID::Demon,     "assets/sprites/player.png"},
  {SPRITE_ID::Rock,      "assets/sprites/box.png"},
  {SPRITE_ID::Ground,    "assets/sprites/ground.png"},
  {SPRITE_ID::Medusa,    "assets/sprites/medusa.png"},
};

Sprite* GetSpriteFromID(ID id, Sprite* spriteBuffer){
  switch(id){
    case ID::NONE:
      return nullptr;
    case ID::GROUND:
      return &spriteBuffer[(int)SPRITE_ID::Ground];
    case ID::WALL:
      return &spriteBuffer[(int)SPRITE_ID::Wall];
    case ID::DEMON:
      return &spriteBuffer[(int)SPRITE_ID::Demon];
    case ID::ROCK:
      return &spriteBuffer[(int)SPRITE_ID::Rock];
    case ID::MEDUSA:
      return &spriteBuffer[(int)SPRITE_ID::Medusa];
    case ID::GHOST:
      return &spriteBuffer[(int)SPRITE_ID::Ghost];
    case ID::GOLEM:
      return &spriteBuffer[(int)SPRITE_ID::Golem];
    default:
      return &spriteBuffer[(int)SPRITE_ID::Fallback];
  }
}


namespace AssetManagement{
  void LoadAllSprites(Sprite *spriteBuffer, SDL_Renderer *renderer){
    for(SpriteDataEntry entry : allSpriteData){
      LoadSprite(spriteBuffer, entry, renderer);
    }
  }

  void LoadSprite(Sprite* spriteBuffer, SpriteDataEntry entry, SDL_Renderer* renderer){
    SDL_Surface* surface = IMG_Load(entry.path);
    if(surface == nullptr){
      surface = IMG_Load(FALLBACK_PATH);
    }
    assert(surface != nullptr);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    Sprite* sprite = &spriteBuffer[(int)entry.id];
    sprite->texture = texture;
    sprite->height = texture->h;
    sprite->width = texture->w;

    SDL_DestroySurface(surface);
  }
  
}

/*
Sprite* AssetManagement::LoadSprite(Memory::Arena* arena, SDL_Renderer* renderer, const char* name){
  string path = DIRECTORY;
  path = path.append(name);
  SDL_Surface* surface = IMG_Load(path.c_str());
  if(surface == nullptr){
    SDL_Log("Could not load %s: %s", path.c_str(), SDL_GetError());
    surface = IMG_Load(FALLBACK);
   }
  if(surface == nullptr){
    SDL_Log("Could not load fallback texture %s: %s", FALLBACK, SDL_GetError());
    return nullptr;
  }
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
  if(texture == nullptr){
    SDL_Log("Could not create texture from %s: %s", path.c_str(), SDL_GetError());
    SDL_DestroySurface(surface);
    return nullptr;
  }
  Sprite* sprite = (Sprite*)Memory::Allocate(arena, sizeof(Sprite));
  if (sprite == nullptr) {
      SDL_Log("Memory::Allocate failed allocating Image");
      SDL_DestroyTexture(texture);
      SDL_DestroySurface(surface);
      return nullptr;
  }
  sprite->texture = texture;
  sprite->height = texture->h;
  sprite->width = texture->w;
  SDL_DestroySurface(surface);
  return sprite;
}
*/
