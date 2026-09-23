#include <cassert>
#include <string>
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_image.h"
#include "spriteLibrary.h"
#include "SDL3/SDL_surface.h"
#include "arena.h"
#include "entity.h"
using namespace std;

const char* DIRECTORY = "assets/sprites/";
const char* FALLBACK_PATH = "assets/sprites/dog.png";

static const SpriteDataEntry allSpriteData[] = {
  {SPRITE_ID::Fallback,  FALLBACK_PATH, 0, 0},
  {SPRITE_ID::Wall,      "assets/sprites/wall.png", 0, 0},
  {SPRITE_ID::Demon,     "assets/sprites/player.png"},
  {SPRITE_ID::Rock,      "assets/sprites/box.png", 10, 20},
  {SPRITE_ID::Ground,    "assets/sprites/ground.png", 0, 0},
  {SPRITE_ID::GroundAlt, "assets/sprites/groundAlt.png", 0, 0},
  {SPRITE_ID::MedusaIdleSide,   "assets/sprites/medusa.png", 12, 24},
  {SPRITE_ID::MedusaIdleFront,  "assets/sprites/medusa.png", 12, 24},
  {SPRITE_ID::MedusaIdleBack,   "assets/sprites/medusa.png", 12, 24},
  {SPRITE_ID::DropShadow, "assets/sprites/medusa.png", 8, 8},
};

Sprite* GetSpriteFromID(ID id, Sprite* spriteBuffer){

  Sprite* spriteToReturn = nullptr;
  
  switch(id){
    case ID::NONE:
      spriteToReturn = nullptr;
      break;
    case ID::GROUND:
      spriteToReturn = &spriteBuffer[(int)SPRITE_ID::Ground];
      break;
    case ID::WALL:
      spriteToReturn = &spriteBuffer[(int)SPRITE_ID::Wall];
      break;
    case ID::DEMON:
      spriteToReturn = &spriteBuffer[(int)SPRITE_ID::Demon];
      break;
    case ID::ROCK:
      spriteToReturn = &spriteBuffer[(int)SPRITE_ID::Rock];
      break;
    case ID::MEDUSA:
      spriteToReturn = nullptr;
      //spriteToReturn = &spriteBuffer[(int)SPRITE_ID::Medusa];
      break;
    case ID::SIREN:
      spriteToReturn = &spriteBuffer[(int)SPRITE_ID::Siren];
      break;
    case ID::GOLEM:
      spriteToReturn = &spriteBuffer[(int)SPRITE_ID::Golem];
      break;
    default:
      spriteToReturn = &spriteBuffer[(int)SPRITE_ID::Fallback];
      break;
  }

  if(spriteToReturn == nullptr || spriteToReturn->texture == nullptr){
    spriteToReturn = &spriteBuffer[(int)SPRITE_ID::Fallback];
  }

  return spriteToReturn;
}

Sprite* GetSpriteFromEntityState(Entity* entity, Sprite* spriteBuffer){
  if(HasBehaviour(entity, Behaviour::IS_PETRIFIED)){
    return &spriteBuffer[(int)SPRITE_ID::Rock];
  }

  switch(entity->id){
    case ID::MEDUSA:
      switch (entity->facing){
        case Direction::RIGHT:
        case Direction::LEFT:
          return &spriteBuffer[(int)SPRITE_ID::MedusaIdleSide];
        case Direction::DOWN:
          return &spriteBuffer[(int)SPRITE_ID::MedusaIdleFront];
        case Direction::UP:
          return &spriteBuffer[(int)SPRITE_ID::MedusaIdleBack];
      }
      default:
        return GetSpriteFromID(entity->id, spriteBuffer);
  }
  return nullptr;
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

    if(entry.pivotX == NOT_SET || entry.pivotY == NOT_SET){
      sprite->pivotX = sprite->width / 2;
      sprite->pivotY = sprite->height / 2;
    }
    else{
      sprite->pivotX = entry.pivotX;
      sprite->pivotY = entry.pivotY;
    }

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
