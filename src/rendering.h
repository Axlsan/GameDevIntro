#pragma once
#include "SDL3/SDL_render.h"
#include "spriteLibrary.h"
#include "camera.h"
#include "levels.h"

void RenderSprite_World(Sprite* sprite, SDL_Renderer* renderer, const Camera* camera, float xPos, float yPos, float scale = 1, float alpha = 1);
void RenderSprite_Grid(Sprite* sprite, LevelData* lvl, SDL_Renderer* renderer, const Camera* camera, float xPos, float yPos, float scale = 1, float alpha = 1);
