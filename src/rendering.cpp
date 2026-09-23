#include "rendering.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "camera.h"
#include "common.h"
#include "levels.h"

void RenderSprite_World(Sprite* sprite, SDL_Renderer* renderer, const Camera* camera, float xPos, float yPos, float scale, float alpha, bool flipped){
  SDL_FRect rect;
  rect.x = xPos;
  rect.y = yPos;
  float finalScale = UPSCALE_FACTOR * scale;
  rect.h = sprite->height * finalScale;
  rect.w = sprite->width  * finalScale;
  rect.x -= sprite->pivotX * finalScale;
  rect.y -= sprite->pivotY * finalScale;
  rect.x -= camera->cameraX;
  rect.y -= camera->cameraY;

  //SDL_SetTextureScaleMode(sprite->texture, SDL_SCALEMODE_PIXELART)
  SDL_SetTextureAlphaModFloat(sprite->texture, alpha); //alpha fo preview in lvl editor

  SDL_RenderTextureRotated(renderer, sprite->texture, NULL, &rect, 0.0, nullptr, flipped ? SDL_FlipMode::SDL_FLIP_HORIZONTAL : SDL_FlipMode::SDL_FLIP_NONE);
}

void RenderSprite_Grid(Sprite *sprite, LevelData* lvl, SDL_Renderer *renderer, const Camera *camera, float xPos, float yPos, float scale, float alpha, bool flipped){
  camera::GridToWorld(&xPos, &yPos, lvl);
  RenderSprite_World(sprite, renderer, camera, xPos, yPos, scale, alpha, flipped);
}

void RenderEntity_OnTile(Sprite* sprite, LevelData* lvl, SDL_Renderer* renderer, const Camera* camera, float xPos, float yPos, float scale, float alpha, bool flipped){
  camera::GridToWorld(&xPos, &yPos, lvl);
  xPos += CELL_SIZE_PX / 2.0;
  yPos += CELL_SIZE_PX / 2.0;
  RenderSprite_World(sprite, renderer, camera, xPos, yPos, scale, alpha, flipped);
}
