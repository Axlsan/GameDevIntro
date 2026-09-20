#include "rendering.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "camera.h"
#include "common.h"
#include "levels.h"

void RenderSprite_World(Sprite* sprite, SDL_Renderer* renderer, const Camera* camera, float xPos, float yPos, float scale){
  SDL_FRect rect;
  rect.x = xPos;
  rect.y = yPos;
  rect.h = sprite->height * UPSCALE_FACTOR * scale;
  rect.w = sprite->width  * UPSCALE_FACTOR * scale;
  rect.x -= camera->cameraX;
  rect.y -= camera->cameraY;

  SDL_RenderTexture(renderer, sprite->texture, NULL, &rect);
}

void RenderSprite_Grid(Sprite *sprite, LevelData* lvl, SDL_Renderer *renderer, const Camera *camera, float xPos, float yPos, float scale){
  camera::GridToWorld(&xPos, &yPos, lvl);
  RenderSprite_World(sprite, renderer, camera, xPos, yPos, scale);
}
