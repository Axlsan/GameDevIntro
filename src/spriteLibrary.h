#pragma once
#include "SDL3/SDL_render.h"
#include "arena.h"
#include "entity.h"

struct Sprite{
  SDL_Texture* texture;
  int width;
  int height;
  int pivotX;
  int pivotY;
};


enum class SPRITE_ID{
  Fallback,
  Ground,
  GroundAlt,
  Wall,
  Rock,
  Demon,
  Medusa,
  MedusaIdleSide,
  MedusaIdleFront,
  MedusaIdleBack,
  Siren,
  Golem,
  DropShadow
};

const int NOT_SET = -1;

struct SpriteDataEntry{
  SPRITE_ID id;
  const char* path;
  int pivotX = NOT_SET;
  int pivotY = NOT_SET;
};

Sprite* GetSpriteFromID(ID id, Sprite* spriteBuffer);

Sprite* GetSpriteFromEntityState(Entity* entity, Sprite* spriteBuffer);

namespace AssetManagement{
  void LoadSprite(Sprite* spriteBuffer, SpriteDataEntry entry, SDL_Renderer* renderer);
  void LoadAllSprites(Sprite* spriteBuffer, SDL_Renderer* renderer);
}
