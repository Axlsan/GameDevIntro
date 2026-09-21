#pragma once

#include "camera.h"
#include "input.h"
#include "spriteLibrary.h"

struct Editor{
  ID objectToPlaceID;
};

namespace EDITOR{
  void DrawObjectPanel(Editor* editor, Sprite* spriteBuffer);
  void PlaceObject(const int x, const int y, Editor* editor, LevelData* lvl);
  void Update(Editor* editor, Input* input, LevelData* lvl);
  void DrawPreview(Editor* editor, Input* innput, SDL_Renderer* renderer, LevelData* lvl, Camera* camera, Sprite* spriteBuffer);
}
